// lograd.cpp
// g++ -std=c++11 -o lograd lograd.cpp cell.cpp pin.cpp circuit.cpp lgc.cpp numeric.cpp -lm

#include "lograd.h"
#include "cell.h"
#include "lgc.h"
#include "circuit.h"
#include <time.h>

using namespace std;

void dev_prompt(void);
void dev_get_line(char *buffer, FILE *stream);
void dev_menu(void);
//void introspect_gv(circuit* circ);
void introspect_gv(ofstream& outfile, circuit *circ);
char* rgb_to_string(rgb_triplet* rgb);

int main(void){

    srand((unsigned)time(NULL));

    dev_prompt();

    _msg("Done");

}


//
//
nada::nada(int this_id){
    id=this_id;
}


//
//
void dev_prompt(void){

    char *cmd_code_str;
    int   cmd_code=0;
    output_pin* op;

    char buffer[256];

    int i;

    _say("type 0 for menu");
    _msg("XXX: add not_cell");

//    ifstream t("test.lgc");
    bool lgc_file_processed;

    ifstream infile;
    ofstream outfile;

    stringstream *file_buf;
    string_vector* lgc;
    string_map* m;

    cell *c;
    loss_cell *lc;

    circuit *circ;

    vector<cell*>* all_cells;

    std::vector<int> v;

//    float s, x0, x1, u, y0, y1, f;
    float target, actual, error;
    float gamma=10;
    unsigned char r,g,b;

    while(1){

        _prn("% ");

        dev_get_line(buffer, stdin);

        cmd_code_str = strtok(buffer, " ");
        if(cmd_code_str == NULL) continue;
        cmd_code = atoi(cmd_code_str);

        switch(cmd_code){
            case 0:
                dev_menu();
                break;

            case 1:
                _say("cmd_code==1");

//                introspect_gv(circ);

                break;

            case 2:
                _say("exiting");
                return;

            case 3:
                cmd_code_str = strtok(NULL, " ");
                if(cmd_code_str == NULL){ _say("no argument given"); continue; }

                infile.open(cmd_code_str, std::ifstream::in);

                if(!lgc_file_processed){
                    file_buf = new stringstream;
                    *file_buf << infile.rdbuf();

                    lgc = lgc_process2(file_buf);
                    _say("test.lgc read in");
                    lgc_file_processed=true;
                }

                circ = new circuit(lgc);

                // TODO:
                //
                //  * DONE forward_pass()
                //  * DONE update_loss() --> still need to make sure losses propagate back correctly
                //  * backward_pass()
                //
                //      XXX How do we know (for sure) all connections have been updated prior to visiting
                //              each output_pin? Need to verify this vis-a-vis reverse BFS.
                //
                //  * update_vars() --> still need to make sure losses propagate back correctly
                //  * pass_reset() -- clear visited_fwd/bwd for all cells
                //  * iterate(N) -- Performs forward_pass(), backward_pass() and pass_reset for N iterations
                //  * get_current_inputs() -- current solution
                //  * solver() -- iterated passes
                //  * state-dumper (helper fn that dumps to STDOUT)

                break;

            case 4:
                gamma *= 0.9;
                _df(gamma);
                circ->iterate(1,gamma);
                break;

            case 5:
                circ->show_pin_values();
                break;

            case 6:
                circ->show_pin_gradients();
                break;

            case 7:
                circ->show_input_vars();
                break;

            case 8:
                cout << "circuit loss: " << circ->get_circuit_loss() << endl;
                break;

            case 9:
                circ->do_forward_pass();
                break;

            case 10:
                circ->update_loss();
                break;

            case 11:
                circ->do_backward_pass();
                break;

            case 12:
                circ->update_vars();
                break;

            case 13:
                circ->reset_iteration();
                break;

            case 14:
                outfile.open("test.dot", std::ofstream::out);
                introspect_gv(outfile,circ);
//                introspect_gv(circ);
                break;

//            case 4:
//                cmd_code_str = strtok(NULL, " ");
//                if(cmd_code_str == NULL){ _say("no argument given"); continue; }
//                cout << *(string*)((*m)[cmd_code_str]) << endl;
//                break;
//
//            case 5:
//                cmd_code_str = strtok(NULL, " ");
//                if(cmd_code_str == NULL){ _say("no argument given"); continue; }
//                op = (output_pin*)(void*)((*m)[cmd_code_str]);
//                cout << op->get_value() << endl;
//                break;
//
//            case 6:
//                cmd_code_str = strtok(NULL, " ");
//                if(cmd_code_str == NULL){ _say("no argument given"); continue; }
//                c = (cell*)(void*)((*m)[cmd_code_str]);
//                cout << c->get_circ_name() << endl;
//                break;

            default:
                _say("unrecognized cmd_code");
                dev_menu();
                break;
        }

        for(i=0;i<256;i++){ buffer[i]=0; } // zero out the buffer

    }

}


//
//
void dev_get_line(char *buffer, FILE *stream){

    int c, i=0;

    while(1){ //FIXME unsafe, wrong
        c = fgetc(stream);
        if(c == EOF || c == '\n'){
            break;
        }
        buffer[i] = c;
        i++;
    }

    buffer[i] = '\0';

}


//
//
void dev_menu(void){

    _say( "\n0     .....    list command codes\n"
            "1     .....    dev one-off\n"
            "2     .....    exit\n"
            "3     .....    read test.lgc\n"
            "4     .....    circ->iterate(1)\n"
            "5     .....    circ->show_pin_values()\n"
            "6     .....    circ->show_pin_gradients()\n"
            "7     .....    circ->show_input_vars()\n"
            "8     .....    circ->get_circuit_loss()\n"
            "9     .....    circ->do_forward_pass()\n"
            "10    .....    circ->update_loss()\n"
            "11    .....    circ->do_backward_pass()\n"
            "12    .....    circ->update_vars()\n"
            "13    .....    circ->reset_iteration()\n"
            "14    .....    introspect_gv(circ)\n" );

//            "4     .....    string lookup in circ_map\n"
//            "5     .....    output_pin lookup in circ_map\n"
//            "6     .....    cell.circ_name lookup in circ_map\n"

}


//
//
void introspect_gv(ofstream& outfile, circuit *circ){ // introspect_gv#

    cell *c;
    output_pin* op;
    char ccolor[] = "ff8080";
    char* color;

    string this_cell_name;
    string con_cell_name;
    cell_type this_cell_type;

    vector<input_pin*>* connections;
    int num_connections;
    input_pin* con;

    float circuit_loss_gradient;
    float cell_loss_gradient;
    float temperature;

    std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
    std::cout.rdbuf(outfile.rdbuf()); //redirect std::cout to out.txt!

    cout << "digraph babel {" << endl;
    cout << "graph [rankdir = \"LR\"];" << endl;

    circuit_loss_gradient = (circ->get_loss_cell())->get_loss_gradient();
    temperature = 1;
    color = rgb_to_string( temperature_to_rgb(temperature) );
    cout << "loss [label=\"loss=" << circuit_loss_gradient 
         << "\", style=filled, color=\"#" << color << "\"];" << endl;

//    cout << "loss [label=\"loss\",color=\"" << ccolor << "\"];" << endl;

//  for each cell in circ:
//      op = cell->get_f_pin();
//      temperature = logistic( op->get_loss_gradient() );
//      color = rgb_to_string( temperature_to_rgb( temperature ));
//      this_cell_name = cell->get_circ_name();
//      cout <<  this_cell_name << " [label=\"" << this_cell_name 
//           << "\", color=#" << color << "];" << endl;
//
//      for each connection con in op->connections():
//          con_cell_name = (con->this_cell()}->get_circ_name();
//          cout << this_cell_name << " -> " << con_cell_name 
//               << " [label = \"" << con->get_circ_name() << "\"];" << endl;

    vector<cell*>* cells = circ->get_all_cells();
    int num_cells = cells->size();

    for(int i=0; i<num_cells; i++){

        c = (*cells)[i];
        op = c->get_f_pin();

        connections = op->get_connections();
        num_connections = connections->size();

        if(num_connections == 0)
            continue;

        this_cell_name = c->get_circ_name();
        this_cell_type = c->get_type();

        cell_loss_gradient = c->get_loss_gradient();
        temperature = cell_loss_gradient / circuit_loss_gradient;
        color = rgb_to_string( temperature_to_rgb(temperature) );
//    cout << "loss [label=\"loss\", style=filled, color=\"#" << color << "\"];" << endl;

        if(this_cell_type == VAR_CELL 
                || this_cell_type == ACT_CELL
                || this_cell_type == CONST_CELL ){
            cout << this_cell_name << " [label=\"" << this_cell_name
                 << "=" << op->read_value()
                 << "\", style=filled, color=\"#" << color << "\"];" << endl;
        }
        else{
            cout << this_cell_name << " [label=\"" << this_cell_name
                 << "\", style=filled, color=\"#" << color << "\"];" << endl;
        }

        for(int j=0; j<num_connections; j++){
            con = (*connections)[j];
            con_cell_name = (con->get_this_cell())->get_circ_name();
            cout << this_cell_name << " -> " << con_cell_name 
                 << " [label = \"" << con->get_circ_name() << "\"];" << endl;
        }

    }

    cout << "}" << endl;

    vector<var_cell*>* var_cells = circ->get_variable_cells();

    int num_variables = var_cells->size();
//    output_pin* op;
    string name;
    float value,gradient;

    for(int i=0; i<num_variables; i++){
        op = (*var_cells)[i]->get_f_pin();
        name = (*var_cells)[i]->get_circ_name();
        value = op->read_value();
        gradient = op->get_loss_gradient();
        cout << "// " << name 
             << " v=" << value 
             << " g=" << gradient << endl;
    }

    std::cout.rdbuf(coutbuf); //reset to standard output again

}



char* rgb_to_string(rgb_triplet* rgb){
    char* buffer = new char[6];
    char* wrbuf = buffer;
    wrbuf += sprintf(wrbuf,"%02x",rgb->r);
    wrbuf += sprintf(wrbuf,"%02x",rgb->g);
             sprintf(wrbuf,"%02x",rgb->b);
    return buffer;
}

//    string_map* m = circ->get_circ_map();
//
////    c = (*m)["m2"];
//    c = (cell*)(void*)((*m)["m2"]);
//    op = c->get_f_pin();    
//    op = c->get_f_pin();    
//    connections = op->get_connections();
//    num_connections = connections->size();
//    con = (*connections)[0];
//    con_cell_name = (con->get_this_cell())->get_circ_name();
//cout << con_cell_name << endl;

//                cmd_code_str = strtok(NULL, " ");
//                if(cmd_code_str == NULL){ _say("not enough arguments"); continue; }
//                target = atof(cmd_code_str);

//                // 0 <= target <= 1
//                if(target<0.5){
//                    //target == 0,   R=0  , G=0  , B=255
//                    //target == 0.5, R=128, G=128, B=255
//                    r=256*target + 64;
//                    g=256*target + 64;
//                    b=255;
//                }
//                else{
//                    //target == 0.5+delta,   R=128, G=128, B=255
//                    //target == 1.0      ,   R=0  , G=0  , B=255
//                    r=255;
//                    g=256*(1-target) + 64; // 192 .. 64
//                    b=256*(1-target) + 64;
//                }
//
//_d(r);
//_d(g);
//_d(b);

//                cmd_code_str = strtok(NULL, " ");
//                if(cmd_code_str == NULL){ _say("not enough arguments"); continue; }
//                actual = atof(cmd_code_str);
//
//                target = (target/2)+0.5;
//                actual = (actual/2)+0.5;
//
//                error = fabs(target-actual);
//
//                error = error*error;
//
//                _df(error);

//                cmd_code_str = strtok(NULL, " ");
//                if(cmd_code_str == NULL){ _say("not enough arguments"); continue; }
//                s = atof(cmd_code_str);
//
//                cmd_code_str = strtok(NULL, " ");
//                if(cmd_code_str == NULL){ _say("not enough arguments"); continue; }
//                x0 = atof(cmd_code_str);
//
//                cmd_code_str = strtok(NULL, " ");
//                if(cmd_code_str == NULL){ _say("not enough arguments"); continue; }
//                x1 = atof(cmd_code_str);
//
//                u  =  (s + 1) / 2;
//                y0 = (x0 + 1) / 2;
//                y1 = (x1 + 1) / 2;
//
//                f = (2 * (u*y1 + (1-u)*y0) - 1);
//                _df(f);




// Clayton Bauman 2018

