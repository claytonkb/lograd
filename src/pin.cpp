// pin.cpp

#include "lograd.h"
#include "pin.h"
#include "cell.h"


                    //////////////////////////////
                    //        PIN (BASE)        //
                    //////////////////////////////

pin::pin(){
    reset_pin_state();
    value = 1; //logistic(rand_range(-2,2));
    loss_gradient = 0; //logistic(rand_range(-2,2));
    reset_pin_visited();
}

void pin::init_value(pin_init pi){
    value = acme_rand(pi);
}

float pin::get_value(void){
    return value;
}

void  pin::set_value(float x){
    value = x;
}

float pin::get_loss_gradient(void){
    return loss_gradient;
}

void pin::set_loss_gradient(float x){
    loss_gradient = x;
}

void pin::accumulate_loss_gradient(float x){
    if(state==ACCUMULATE){
        loss_gradient+=x;
    }
    else{ // (state==OVERWRITE)
        loss_gradient=x;
        state=ACCUMULATE;
    }
}

void pin::reset_pin_state(void){ // network update needs to call this to set for next update
    state=OVERWRITE;
}

cell* pin::get_this_cell(void){
    return this_cell;
}

void pin::set_this_cell(cell* c){
    this_cell = c;
}

pin_type pin::get_type(void){
    return type;
}

string pin::get_circ_name(void){
    return circ_name;
}

void pin::set_circ_name(string s){
    circ_name = s;
}

void pin::forward_update(void){
    if(visited_fwd) return;
    visited_fwd = true;
}

void pin::backward_update(void){
    if(visited_bwd) return;
    visited_bwd = true;
}

void pin::reset_pin_visited(void){
    visited_fwd = false;
    visited_bwd = false;
}

float pin::read_value(void){
    return value;
}



                    //////////////////////////////
                    //        OUTPUT PIN        //
                    //////////////////////////////

output_pin::output_pin(){
    connections = new vector<input_pin*>;
    dynamic_pin=false;
    init=INIT_RAND;
    type=OUTPUT_PIN;
}

output_pin::output_pin(pin_init pi){
    init_value(pi);
    connections = new vector<input_pin*>;
    dynamic_pin=false;
    init=pi;
    type=OUTPUT_PIN;
}

output_pin::output_pin(pin_init pi, bool dynamic=false){
    connections = new vector<input_pin*>;
    dynamic_pin=dynamic;
    init=pi;
    type=OUTPUT_PIN;
}

// for every pin in list of connections:
//      invoke connections[i].set_value(this.value);
void output_pin::source_value(void){

    int num_connections = connections->size();

    if(dynamic_pin==false){
        for(int i=0; i<num_connections; i++){
            (*connections)[i]->set_value(this->value);
        }
    }
    else{ //dynamic_pin==true 
        _say("unimplemented");
    }

}

float output_pin::get_value(void){

//    float result;
//
//    if(dynamic_pin==false){
//        result = value;
//    }
//    else{ // dynamic_pin==true
//        result = acme_rand(init);
//    }
//
//    return result;

    if(dynamic_pin==true){
        value = acme_rand(init);
    }

    return value;

}

// for every pin in list of connections:
//      this.loss_gradient = sum[ connections[i].get_loss_gradient() ]
void output_pin::sink_loss_gradient(void){

    this->loss_gradient = 0.0f;
    int num_connections = connections->size();

    for(int i=0; i<num_connections; i++){
        this->loss_gradient += (*connections)[i]->get_loss_gradient();
    }

}

void output_pin::add_connection_to_pin(input_pin* pin){
    connections->push_back(pin);
}

vector<input_pin*>* output_pin::get_connections(void){
    return connections;
}


// only call this once all input_pins in this_cell have been forward_updated
void output_pin::forward_update(void){
    // recalculate this->value based on this_cell type [and inputs]
    switch(this_cell->get_type()){ // XXX pretty sure there's a clean way to do this with polymorphism
        case VAR_CELL:
            ((var_cell*)this_cell)->forward_propagate();
            break;
        case CONST_CELL:
            ((const_cell*)this_cell)->forward_propagate();
            break;
        case RAND_CELL:
            ((rand_cell*)this_cell)->forward_propagate();
            break;
        case ACT_CELL:
            ((act_cell*)this_cell)->forward_propagate();
            break;
        case SOFT_MUX_CELL:
            ((soft_mux_cell*)this_cell)->forward_propagate();
            break;
        case LOSS_CELL:
            ((loss_cell*)this_cell)->forward_propagate();
            break;
    }
}


void output_pin::backward_update(void){

    //  for(connections)
    //      gradient_sum += (*connections)[i]
    loss_gradient=0;
//cout << circ_name << endl;
    int num_connections = connections->size();
    input_pin* ip;
//_dd(num_connections);
    for(int i=0;i<num_connections; i++){
        ip = (*connections)[i];
        loss_gradient += ip->get_loss_gradient();
    }
//_df(loss_gradient);

    // kick off this_cell->backward_propagate(), which updates each input_pin's
    //      loss_gradient

    this_cell->backward_propagate();

}


                    //////////////////////////////
                    //         INPUT PIN        //
                    //////////////////////////////

input_pin::input_pin(){
    connected=false;
    type=INPUT_PIN;
}

void input_pin::sink_value(void){
    value = connection->get_value();
}

void input_pin::source_loss_gradient(void){
    connection->accumulate_loss_gradient(value);
}

bool input_pin::connect_to_pin(output_pin* pin){

    if(connected)
        return false;

    connection = pin;
    connected = true;
    return true;

}

bool input_pin::is_connected(void){
    return connected;
}

output_pin* input_pin::get_connection(void){
    return connection;
}

void input_pin::forward_update(void){
    sink_value();
}

void input_pin::backward_update(void){
    // apply partial derivative of this pin w.r.t. output_pin
    //      to gradient_sum
    // update loss_gradient

//    float cell_gradient = this_cell->get_loss_gradient();
//    loss_gradient = sqrt(incoming_loss_gradient) + rand_range(-0.1,0.1);

}

void input_pin::set_incoming_loss_gradient(float x){
    incoming_loss_gradient = x;
}


// Clayton Bauman 2018

