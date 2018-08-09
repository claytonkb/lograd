// cell.cpp

#include "lograd.h"
#include "cell.h"
#include <vector>

using namespace std;

                    //////////////////////////////
                    //        CELL (BASE)       //
                    //////////////////////////////

cell::cell(string name){
    visited_fwd = false;
    visited_bwd = false;
    inputs  = new vector<input_pin*>();
    outputs = new vector<output_pin*>();
    circ_name = name;
}

cell::~cell(){
    delete inputs;
    delete outputs;
}

void cell::reset_visited(void){
    visited_fwd = false;
    visited_bwd = false;
}

void cell::set_visited_fwd(void){
    visited_fwd = true;
}

void cell::set_visited_bwd(void){
    visited_bwd = true;
}

output_pin* cell::get_f_pin(void){
    return f;
}


void cell::set_f_pin(output_pin* f_pin){
    f = f_pin;
    outputs->push_back(f_pin);
}

string cell::get_circ_name(void){
    return circ_name;
}

cell_type cell::get_type(void){
    return type;
}

vector<input_pin*>* cell::get_inputs(void){
    return inputs;
}

vector<output_pin*>* cell::get_outputs(void){
    return outputs;
}

bool cell::is_visited_fwd(void){
    return visited_fwd;
}

bool cell::is_visited_bwd(void){
    return visited_bwd;
}

void cell::forward_propagate(void){
    if(visited_fwd) return;
    set_visited_fwd();
}

void cell::backward_propagate(void){
    if(visited_bwd) return;
    set_visited_bwd();
}

float cell::get_loss_gradient(void){
    return f->get_loss_gradient();
}


                    //////////////////////////////
                    //       SOFT MUX CELL      //
                    //////////////////////////////

input_pin* soft_mux_cell::get_s_pin(void){
    return s;
}

void soft_mux_cell::set_s_pin(input_pin* s_pin){
    s = s_pin;
    inputs->push_back(s_pin);
}

input_pin* soft_mux_cell::get_x0_pin(void){
    return x0;
}

void soft_mux_cell::set_x0_pin(input_pin* x0_pin){
    x0 = x0_pin;
    inputs->push_back(x0_pin);
}

input_pin* soft_mux_cell::get_x1_pin(void){
    return x1;
}

void soft_mux_cell::set_x1_pin(input_pin* x1_pin){
    x1 = x1_pin;
    inputs->push_back(x1_pin);
}

void soft_mux_cell::forward_propagate(void){
}

void soft_mux_cell::backward_propagate(void){
}


                    //////////////////////////////
                    //        CONST CELL        //
                    //////////////////////////////

void const_cell::forward_propagate(void){
}


void const_cell::backward_propagate(void){
}

                    //////////////////////////////
                    //         RAND CELL        //
                    //////////////////////////////

void rand_cell::forward_propagate(void){
}

void rand_cell::backward_propagate(void){
}


                    //////////////////////////////
                    //          VAR CELL        //
                    //////////////////////////////

void var_cell::set_var(float x){
    f->set_value(x);
}

void var_cell::connect_f(input_pin* pin){
//    f->add_connection_to_pin(pin);
}

void var_cell::forward_propagate(void){
}

void var_cell::backward_propagate(void){
}


                    //////////////////////////////
                    //          ACT CELL        //
                    //////////////////////////////

// NOTE: we want a -1,1 squashing activation function
// This will keep the network parameters balanced around zero which should be
//    especially important when dealing with highly random functions
// Current choice is softsign(x) = 1 / 1+|x|
//      df/dx(softsign) = 1 / (1+|x|)^2
// Other -1,1 squashing activation functions:
//      * tanh
//      * arctan (kind of)
//      * ISRU   (kind of)
//      * 2*sigmoid(x)-1
// See Wiki (activation_function)

input_pin* act_cell::get_x_pin(void){
    return x;
}

void act_cell::set_x_pin(input_pin* x_pin){
    x = x_pin;
    inputs->push_back(x_pin);
}

void act_cell::connect_x(output_pin* pin){
    x->connect_to_pin(pin);
}

void act_cell::connect_f(input_pin* pin){
    f->add_connection_to_pin(pin);
}

void act_cell::forward_propagate(void){
    f->set_value(act_fn(x->get_value()));
}

void act_cell::backward_propagate(void){
}


                    //////////////////////////////
                    //         LOSS CELL        //
                    //////////////////////////////

void loss_cell::connect_x(output_pin* pin, float target){
    loss_pin_target* pt = new loss_pin_target;
    pt->pin = pin;
    pt->target = target;
    connections->push_back(pt);
}

//void loss_cell::source_loss(void){
//
//    float curr_loss = get_total_loss();
////
////    vector<loss_pin_target*>* connections = x->get_connections();
//    int num_connections = connections->size();
//    output_pin* op;
//_df(curr_loss);
//_dd(num_connections);
//    for(int i=0;i<num_connections; i++){
//        op = ((*connections)[i])->pin;
//fprintf(stderr,"%s\n", (op->get_circ_name()).c_str());
//        op->set_loss_gradient(curr_loss);
//    }
//
//cout << op->get_loss_gradient() << endl;
//
//}

float loss_cell::get_total_loss(void){
    calculate_total_loss();
    return total_loss;
}

void loss_cell::calculate_total_loss(void){

//    vector<loss_pin_target*>* connections = x->get_connections();

    int num_connections = connections->size();
    int i;

    loss_pin_target *lpt;

    float value;

    float diff;
    vector<float> diffs;

    for(i=0;i<num_connections;i++){
        lpt = (*connections)[i];
        value = (lpt->pin)->get_value();
        diff = value - lpt->target;

//_df(value);
//_df(lpt->target);

        diffs.push_back( diff );
    }

    total_loss = mean_sq_error(&diffs);
_df(total_loss);
    x->set_loss_gradient(total_loss);

}

void loss_cell::forward_propagate(void){}

void loss_cell::backward_propagate(void){}

input_pin* loss_cell::get_x_pin(void){
    return x;
}



// Clayton Bauman 2018

