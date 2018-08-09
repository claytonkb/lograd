// lograd.cpp
// g++ -std=c++11 -o lograd lograd.cpp cell.cpp pin.cpp circuit.cpp lgc.cpp numeric.cpp -lm

#include "lograd.h"
#include "cell.h"
#include "lgc.h"
#include "circuit.h"
#include <time.h>

void dev_prompt(void);
void dev_get_line(char *buffer, FILE *stream);
void dev_menu(void);

using namespace std;


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
    _msg("load circ_map before testing!!!");

    ifstream t("test.lgc");
    bool lgc_file_processed;

    stringstream *file_buf;
    string_vector* lgc;
    string_map* m;

    cell *c;
    loss_cell *lc;

    circuit *circ;

    vector<cell*>* all_cells;

    std::vector<int> v;

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
                break;

            case 2:
                _say("exiting");
                return;

            case 3:
                if(!lgc_file_processed){
                    file_buf = new stringstream;
                    *file_buf << t.rdbuf();

                    lgc = lgc_process2(file_buf);
                    _say("test.lgc read in");
                    lgc_file_processed=true;
                }

                circ = new circuit(lgc);

//                circ = new circuit;
//
//                circ->init_circ_map(lgc);
//                circ->init_pin_list();

//                circ->iterate(1);

//                circ->do_forward_pass();
//                circ->update_loss();
//                circ->do_backward_pass();

//                fprintf(stderr,"total_loss: %f\n", (circ->get_loss_cell())->get_total_loss());

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
                cmd_code_str = strtok(NULL, " ");
                if(cmd_code_str == NULL){ _say("no argument given"); continue; }
                cout << *(string*)((*m)[cmd_code_str]) << endl;
                break;

            case 5:
                cmd_code_str = strtok(NULL, " ");
                if(cmd_code_str == NULL){ _say("no argument given"); continue; }
                op = (output_pin*)(void*)((*m)[cmd_code_str]);
                cout << op->get_value() << endl;
                break;

            case 6:
                cmd_code_str = strtok(NULL, " ");
                if(cmd_code_str == NULL){ _say("no argument given"); continue; }
                c = (cell*)(void*)((*m)[cmd_code_str]);
                cout << c->get_circ_name() << endl;
                break;

            case 7:
                circ->iterate(1);
                break;

            case 8:
                circ->show_pin_values();
                break;

            case 9:
                circ->show_pin_gradients();
                break;

            case 10:
                circ->show_input_vars();
                break;

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
            "4     .....    string lookup in circ_map\n"
            "5     .....    output_pin lookup in circ_map\n"
            "6     .....    cell.circ_name lookup in circ_map\n"
            "7     .....    circ->iterate(1)\n"
            "8     .....    circ->show_pin_values()\n"
            "9     .....    circ->show_pin_gradients()\n" );

}



// Clayton Bauman 2018

