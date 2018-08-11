// circuit.cpp

#include "lograd.h"
#include "circuit.h"

const string f_pin  = string(".f");
const string s_pin  = string(".s");
const string x_pin  = string(".x");
const string x0_pin = string(".x0");
const string x1_pin = string(".x1");
const string type   = string(".type");

const string* type_var_cell   = new string("var_cell");
const string* type_const_cell = new string("const_cell");
const string* type_rand_cell  = new string("rand_cell");
const string* type_act_cell   = new string("act_cell");
const string* type_mux_cell   = new string("mux_cell");
const string* nil             = new string("nil");


////////////////////////////////////////////////////////////////////////////
// CIRCUIT
////////////////////////////////////////////////////////////////////////////

//
//
circuit::circuit(){

    mode_select     = FORWARD;

    all_cells       = new std::vector<cell*>();
    input_cells     = new std::vector<cell*>();
    prop_cells      = new std::vector<cell*>();
    output_cells    = new std::vector<cell*>();
    variables       = new std::vector<var_cell*>();

    circuit_loss    = new loss_cell("loss");

    pin_list        = new vector<pin*>;

    circ_map        = new string_map;

}

//
//
circuit::circuit(string_vector* lgc){

    mode_select     = FORWARD;

    all_cells       = new std::vector<cell*>();
    input_cells     = new std::vector<cell*>();
    prop_cells      = new std::vector<cell*>();
    output_cells    = new std::vector<cell*>();
    variables       = new std::vector<var_cell*>();

    circuit_loss    = new loss_cell("loss");

    pin_list        = new vector<pin*>;

    circ_map        = new string_map;

    init_circ_map(lgc);
    init_pin_list();

}


//
//
void circuit::iterate(int n){

    for(int i=0;i<n;i++){

        do_forward_pass();
        update_loss();
        do_backward_pass();
        update_vars();
        reset_iteration();

    }

}

//
//
void circuit::iterate(int n, float g){

    for(int i=0;i<n;i++){

        do_forward_pass();
        update_loss();
        do_backward_pass();
        update_vars(g);
        reset_iteration();

    }

}

//
//
void circuit::do_forward_pass(void){

    int pin_list_size = pin_list->size();

    for(int i=0;i<pin_list_size;i++){
        ((*pin_list)[i])->forward_update();
    }

}


//
//
void circuit::do_backward_pass(void){

    int pin_list_size = pin_list->size();

    for(int i=pin_list_size-1;i>=0;i--){
        ((*pin_list)[i])->backward_update();
    }

}

// XXX Note:
//      We might want a better naming convention for loss_cell and input_cells
//      during update of the loss function and variables, based on gradients

//
//
void circuit::update_loss(void){
    circuit_loss->calculate_total_loss();
}

//
//
void circuit::update_vars(void){

    int num_var_cells = variables->size();

//    output_pin* op;
//    var_cell* curr_var;

    for(int i=0;i<num_var_cells;i++){

        (*variables)[i]->update();

//        curr_var = (*variables)[i];
//        op = curr_var->get_f_pin();
//        cout << curr_var->get_type() << endl;

//        if(curr_var->get_type() == VAR_CELL){
//            curr_var->update_var();
//        }
    }

}


//
//
void circuit::update_vars(float g){

    int num_var_cells = variables->size();

    for(int i=0;i<num_var_cells;i++){
        (*variables)[i]->update(g);
    }

}


//
//
void circuit::reset_iteration(void){

    // for each cell in all_cells:
    //      cell->reset_visited()

    int num_cells = all_cells->size();
    cell* curr_cell;

    for(int i=0; i<num_cells; i++){
        curr_cell = (*all_cells)[i];
        curr_cell->reset_visited();
    }

}


//
//
void circuit::init_circ_map(string_vector* lgc){

    string_map* m = circ_map;//new string_map;

    int lgc_size = lgc->size();

    int i=0;

    if( (*lgc)[i++] != ".cel"){
        fprintf(stderr,"Expected .cel, found: %s\n", (*lgc)[0].c_str());
        _die;
    }

    size_t dot_prefix0, dot_prefix1;

    cell*       c;
    output_pin* f;
    input_pin*  x;
    input_pin*  s;
    input_pin*  x0;
    input_pin*  x1;

    ////////////////////////////////////////////////////////////////////////////
    // CELLS
    ////////////////////////////////////////////////////////////////////////////

    var_cell* vc;

    if( (*lgc)[i] == ".var"){

        i++;

        dot_prefix0 = first_char_dot((*lgc)[i]);
        dot_prefix1 = first_char_dot((*lgc)[i+1]);

        while(    not_section_label(dot_prefix0) 
               && not_section_label(dot_prefix1) ){

            vc = new var_cell((string)(*lgc)[i]);
//            vc->dev_id = i;

//            f = new output_pin(INIT_RAND);

            if((*lgc)[i+1] == "0"){
                f = new output_pin(INIT_ZERO);
            }
            else if((*lgc)[i+1] == "1"){
                f = new output_pin(INIT_ONE);
            }
            else if((*lgc)[i+1] == "-1"){
                f = new output_pin(INIT_NEG_ONE);
            }
            else if((*lgc)[i+1] == "r"){
                f = new output_pin(INIT_RAND);
            }
            else if((*lgc)[i+1] == "r1"){
                f = new output_pin(INIT_RAND01);
            }
            else if((*lgc)[i+1] == "r2"){
                f = new output_pin(INIT_RAND11);
            }
            else if((*lgc)[i+1] == "h1"){
                f = new output_pin(INIT_HI1);
            }
            else if((*lgc)[i+1] == "h2"){
                f = new output_pin(INIT_HI2);
            }
            else if((*lgc)[i+1] == "l1"){
                f = new output_pin(INIT_LO1);
            }
            else if((*lgc)[i+1] == "l2"){
                f = new output_pin(INIT_LO2);
            }
            else{
                fprintf(stderr,"LOGRAD: Unknown initializer, defaulting to RAND11: %s\n", (*lgc)[i+1].c_str());
                f = new output_pin(INIT_RAND01);
            }


            f->set_this_cell(vc);
            f->set_circ_name((*lgc)[i]+f_pin);
            vc->set_f_pin(f);

            (*m)[(*lgc)[i]+f_pin] = f;

            (*m)[(*lgc)[i]] = vc;
            (*m)[(*lgc)[i]+type] = type_var_cell;

            add_cell(vc,INPUT_ROLE,VAR_CELL);

            i+=2;

            dot_prefix0 = first_char_dot((*lgc)[i]);
            dot_prefix1 = first_char_dot((*lgc)[i+1]);

        }

    }

    const_cell* cc;

    if( (*lgc)[i] == ".con"){

        i++;

        dot_prefix0 = first_char_dot((*lgc)[i]);
        dot_prefix1 = first_char_dot((*lgc)[i+1]);

        while(    not_section_label(dot_prefix0) 
               && not_section_label(dot_prefix1) ){

            cc = new const_cell((string)(*lgc)[i]);
//            cc->dev_id = i;

            if((*lgc)[i+1] == "0"){
                f = new output_pin(INIT_ZERO);
            }
            else if((*lgc)[i+1] == "1"){
                f = new output_pin(INIT_ONE);
            }
            else if((*lgc)[i+1] == "-1"){
                f = new output_pin(INIT_NEG_ONE);
            }
            else if((*lgc)[i+1] == "r"){
                f = new output_pin(INIT_RAND);
            }
            else if((*lgc)[i+1] == "r1"){
                f = new output_pin(INIT_RAND01);
            }
            else if((*lgc)[i+1] == "r2"){
                f = new output_pin(INIT_RAND11);
            }
            else if((*lgc)[i+1] == "h1"){
                f = new output_pin(INIT_HI1);
            }
            else if((*lgc)[i+1] == "h2"){
                f = new output_pin(INIT_HI2);
            }
            else if((*lgc)[i+1] == "l1"){
                f = new output_pin(INIT_LO1);
            }
            else if((*lgc)[i+1] == "l2"){
                f = new output_pin(INIT_LO2);
            }
            else{
                fprintf(stderr,"LOGRAD: Unknown initializer, defaulting to RAND11: %s\n", (*lgc)[i+1].c_str());
                f = new output_pin(INIT_RAND01);
            }

            f->set_this_cell(cc);
            f->set_circ_name((*lgc)[i]+f_pin);
            cc->set_f_pin(f);

            (*m)[(*lgc)[i]+f_pin] = f;

            (*m)[(*lgc)[i]] = cc;
            (*m)[(*lgc)[i]+type] = type_const_cell;

            add_cell(cc,INPUT_ROLE,CONST_CELL);

            i+=2;

            dot_prefix0 = first_char_dot((*lgc)[i]);
            dot_prefix1 = first_char_dot((*lgc)[i+1]);

        }

    }

    rand_cell* rc;

    if( (*lgc)[i] == ".ran"){

        i++;

        dot_prefix0 = first_char_dot((*lgc)[i]);
        dot_prefix1 = first_char_dot((*lgc)[i+1]);

        while(    not_section_label(dot_prefix0) 
               && not_section_label(dot_prefix1) ){

            rc = new rand_cell((string)(*lgc)[i]);
//            rc->dev_id = i;

//            if((*lgc)[i+1] == "0"){
//                f = new output_pin(INIT_LO,true);
//            }
//            else if((*lgc)[i+1] == "1"){
//                f = new output_pin(INIT_HI,true);
//            }
//            else if((*lgc)[i+1] == "r"){
//                f = new output_pin(INIT_RAND11,true);
//            }
//            else{
//                fprintf(stderr,"LOGRAD: Unknown initializer, defaulting to RAND11: %s\n", (*lgc)[i+1].c_str());
//                f = new output_pin(INIT_RAND01,true);
//            }

            if((*lgc)[i+1] == "0"){
                f = new output_pin(INIT_ZERO,true);
            }
            else if((*lgc)[i+1] == "1"){
                f = new output_pin(INIT_ONE,true);
            }
            else if((*lgc)[i+1] == "-1"){
                f = new output_pin(INIT_NEG_ONE,true);
            }
            else if((*lgc)[i+1] == "r"){
                f = new output_pin(INIT_RAND,true);
            }
            else if((*lgc)[i+1] == "r1"){
                f = new output_pin(INIT_RAND01,true);
            }
            else if((*lgc)[i+1] == "r2"){
                f = new output_pin(INIT_RAND11,true);
            }
            else if((*lgc)[i+1] == "h1"){
                f = new output_pin(INIT_HI1,true);
            }
            else if((*lgc)[i+1] == "h2"){
                f = new output_pin(INIT_HI2,true);
            }
            else if((*lgc)[i+1] == "l1"){
                f = new output_pin(INIT_LO1,true);
            }
            else if((*lgc)[i+1] == "l2"){
                f = new output_pin(INIT_LO2,true);
            }
            else{
                fprintf(stderr,"LOGRAD: Unknown initializer, defaulting to RAND11: %s\n", (*lgc)[i+1].c_str());
                f = new output_pin(INIT_RAND01);
            }

            f->set_this_cell(rc);
            f->set_circ_name((*lgc)[i]+f_pin);
            rc->set_f_pin(f);

            (*m)[(*lgc)[i]+f_pin] = f;

            (*m)[(*lgc)[i]] = rc;
            (*m)[(*lgc)[i]+type] = type_rand_cell;

            add_cell(rc,INPUT_ROLE,RAND_CELL);

            i+=2;

            dot_prefix0 = first_char_dot((*lgc)[i]);
            dot_prefix1 = first_char_dot((*lgc)[i+1]);

        }

    }

    act_cell* lc;

    if( (*lgc)[i] == ".act"){

        i++;
        dot_prefix0 = first_char_dot((*lgc)[i]);

        while( not_section_label(dot_prefix0) ){

            lc = new act_cell((string)(*lgc)[i]);
//            lc->dev_id = i;

            f = new output_pin(INIT_RAND11);
            f->set_circ_name((*lgc)[i]+f_pin);
            x = new input_pin();
            x->set_circ_name((*lgc)[i]+x_pin);

            f->set_this_cell(lc);
            x->set_this_cell(lc);

            lc->set_f_pin(f);
            lc->set_x_pin(x);

            (*m)[(*lgc)[i]+f_pin] = f;
            (*m)[(*lgc)[i]+x_pin] = x;

            (*m)[(*lgc)[i]] = lc;
            (*m)[(*lgc)[i]+type] = type_act_cell;

            add_cell(lc,PROPAGATE_ROLE,ACT_CELL);

            i+=1;
            dot_prefix0 = first_char_dot((*lgc)[i]);

        }

    }

    soft_mux_cell* mc;
    string *test;

    if( (*lgc)[i] == ".mux"){

        i++;
        dot_prefix0 = first_char_dot((*lgc)[i]);

        while( not_section_label(dot_prefix0) ){

            mc = new soft_mux_cell((string)(*lgc)[i]);
//            mc->dev_id = i;

            f  = new output_pin(INIT_RAND11);
            s  = new input_pin();
            x0 = new input_pin();
            x1 = new input_pin();

            f->set_circ_name((*lgc)[i]+f_pin);
            s->set_circ_name((*lgc)[i]+s_pin);
            x0->set_circ_name((*lgc)[i]+x0_pin);
            x1->set_circ_name((*lgc)[i]+x1_pin);

             f->set_this_cell(mc);
             s->set_this_cell(mc);
            x0->set_this_cell(mc);
            x1->set_this_cell(mc);

            mc->set_f_pin(f);
            mc->set_s_pin(s);
            mc->set_x0_pin(x0);
            mc->set_x1_pin(x1);

            (*m)[(*lgc)[i]+f_pin]  = f;
            (*m)[(*lgc)[i]+s_pin]  = s;
            (*m)[(*lgc)[i]+x0_pin] = x0;
            (*m)[(*lgc)[i]+x1_pin] = x1;

            (*m)[(*lgc)[i]] = mc;
            (*m)[(*lgc)[i]+type] = type_mux_cell;

            add_cell(mc,PROPAGATE_ROLE,SOFT_MUX_CELL);

            i+=1;
            dot_prefix0 = first_char_dot((*lgc)[i]);

        }

    }

    ////////////////////////////////////////////////////////////////////////////
    // WIRES
    ////////////////////////////////////////////////////////////////////////////

    output_pin* op;
    input_pin*  ip;

    if( (*lgc)[i] != ".wir"){
        fprintf(stderr,"Expected .wir, found: %s\n", (*lgc)[i].c_str());
        _die;
    }

    i++;

    dot_prefix0 = first_char_dot((*lgc)[i]);
    dot_prefix1 = first_char_dot((*lgc)[i+1]);

    while(    not_section_label(dot_prefix0) 
           && not_section_label(dot_prefix1) ){

        // check if pins exist
        if(m->count((*lgc)[i])==0){
            _msg("Pin doesn't exist");
            cout << (*lgc)[i] << endl;
        }
        else if(m->count((*lgc)[i+1])==0){
            _msg("Pin doesn't exist");
            cout << (*lgc)[i+1] << endl;
        }
        else{

            op = (output_pin*)(void*)((*m)[(*lgc)[i]]);
            ip =  (input_pin*)(void*)((*m)[(*lgc)[i+1]]);

            op->add_connection_to_pin(ip);

            if(!ip->connect_to_pin(op)){
                _msg("Multiple connections to input_pin");
                cout << (*lgc)[i+1] << endl;
                _die;
            }

        }

        i+=2;

        dot_prefix0 = first_char_dot((*lgc)[i]);
        dot_prefix1 = first_char_dot((*lgc)[i+1]);

    }

    ////////////////////////////////////////////////////////////////////////////
    // OUTPUTS
    ////////////////////////////////////////////////////////////////////////////

    if( (*lgc)[i] != ".out"){
        fprintf(stderr,"Expected .out, found: %s\n", (*lgc)[i].c_str());
        _die;
    }

    i++;

    dot_prefix0 = first_char_dot((*lgc)[i]);

    while( not_section_label(dot_prefix0) ){

        // Add each named cell to the output_list
        if(     m->count((*lgc)[i])     ==0
            ||  m->count((*lgc)[i]+type)==0){
            _msg("Designated output cell doesn't exist");
            cout << (*lgc)[i] << endl;
            _die;
        }
        else{
            c = (cell*)(void*)((*m)[(*lgc)[i]]);
            designate_output_cell(c);
        }

        i+=1;

        dot_prefix0 = first_char_dot((*lgc)[i]);

    }

    ////////////////////////////////////////////////////////////////////////////
    // LOSS
    ////////////////////////////////////////////////////////////////////////////

    if((*lgc)[i] != ".los"){
        fprintf(stderr,"Expected .los, found: %s\n", (*lgc)[i].c_str());
        _die;
    }

    i++;

    // Create a loss_cell
    loss_cell* loss = get_loss_cell();//new loss_cell("loss");
    float loss_target;

    while(i < lgc_size){

        // Connect each named pin to loss_cell.x, along with its loss target
        op = (output_pin*)(void*)((*m)[(*lgc)[i]]);
        loss_target = atof((*lgc)[i+1].c_str());

        loss->connect_x(op,loss_target);
        op->add_connection_to_pin(loss->get_x_pin());

        i+=2;

    }

    (*m)["loss"] = (void*)loss;

    if(i != lgc_size){
        _warn("LGC file incorrect");
    }

}


//
//
void circuit::init_pin_list(void){//(string_map* circ_map){

    vector<cell*>* output_cells = get_output_cells();
    int num_output_cells = output_cells->size();

    vector<cell*>* working_list = new vector<cell*>;
    vector<cell*>* cell_list = new vector<cell*>;

    int i,j;

    // place all designated output cells into working_list and cell_list
    for(i=0;i<num_output_cells;i++){
        working_list->push_back((cell*)(*output_cells)[i]);
        cell_list->insert(cell_list->begin(),(cell*)(*output_cells)[i]);
    }

    cell *curr_cell;
    vector<input_pin*>* inputs;
    vector<output_pin*>* outputs;
    int num_input_pins;
    output_pin* op;
    cell* ic;

    // traverse circuit breadth-first to build cell_list
    while(working_list->size()>0){

        // curr_cell=pop working_list
        curr_cell = (*working_list)[0];
        working_list->erase(working_list->begin());

        inputs = curr_cell->get_inputs();
        num_input_pins = inputs->size();

        for(i=0;i<num_input_pins;i++){

            op = (*inputs)[i]->get_connection();
            ic = op->get_this_cell();

            if(!ic->is_visited_fwd()){
                working_list->push_back(ic);
                cell_list->insert(cell_list->begin(),ic);
                ic->set_visited_fwd();
            }//else do nothing...

        }

    }

    int cell_list_size = cell_list->size();

    // populate the pins of each cell in cell_list into forward_list
    for(i=0;i<cell_list_size;i++){

        curr_cell = (*cell_list)[i];

        inputs = curr_cell->get_inputs();
        for(j=0;j<inputs->size();j++){
            pin_list->push_back((*inputs)[j]);
        }

        outputs = curr_cell->get_outputs();

        for(j=0;j<outputs->size();j++){
            pin_list_insert((*outputs)[j]);
        }

    }

}


//
//
void circuit::add_cell(cell *b, cell_role r, cell_type t){
    if(t==VAR_CELL){
        variables->push_back((var_cell*)b);
    }
    add_cell(b,r);
}

//
//
void circuit::add_cell(cell *b, cell_role r){

    all_cells->push_back(b);
    switch(r){
        case INPUT_ROLE:
            input_cells->push_back(b);
            break;
        case PROPAGATE_ROLE:
            prop_cells->push_back(b);
            break;
//        case OUTPUT:
//            output_cells->push_back(b);
//            break;
        default:
            _fatal("Can only add input or propagation cells to circuit");
    }
}

//
//
void circuit::designate_output_cell(cell *b){
    output_cells->push_back(b);
}

//
//
loss_cell* circuit::get_loss_cell(void){
    return circuit_loss;
}

//
//
vector<cell*>* circuit::get_all_cells(void){
    return all_cells;
}

//
//
vector<cell*>* circuit::get_output_cells(void){
    return output_cells;
}


//
//
void circuit::pin_list_insert(pin* p){
    pin_list->push_back(p);
}

//
//
vector<pin*>* circuit::get_pin_list(void){
    return pin_list;
}

//
//
void circuit::show_pin_values(void){
    int pin_list_size = pin_list->size();
    pin* p;
    for(int i=0;i<pin_list_size;i++){
        p = (*pin_list)[i];
        cout << p->get_circ_name() << "    " << p->read_value() << endl;
    }
}

//
//
void circuit::show_pin_gradients(void){
    int pin_list_size = pin_list->size();
    pin* p;
    for(int i=0;i<pin_list_size;i++){
        p = (*pin_list)[i];
        cout << p->get_circ_name() << "    " << (float)p->get_loss_gradient() << endl;
    }
}

//
//
void circuit::show_input_vars(void){

    int num_input_cells = input_cells->size();
    cell* c;

    output_pin* op;
    for(int i=0;i<num_input_cells;i++){
        c = (*input_cells)[i];
        op = c->get_f_pin();
        cout << op->get_circ_name() << "    " << op->read_value() << endl;
    }

}

//
//
float circuit::get_circuit_loss(void){
    return circuit_loss->get_total_loss();
}



// Clayton Bauman 2018

