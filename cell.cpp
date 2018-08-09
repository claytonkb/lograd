// cell.cpp

#include "lograd.h"
#include "cell.h"
#include <vector>

using namespace std;


////////////////////////////////////////////////////////////////////////////
// CELL (BASE)
////////////////////////////////////////////////////////////////////////////

//
//
cell::cell(string name){
    visited_fwd = false;
    visited_bwd = false;
    inputs  = new vector<input_pin*>();
    outputs = new vector<output_pin*>();
    circ_name = name;
}

//
//
cell::~cell(){
    delete inputs;
    delete outputs;
}

//
//
void cell::forward_propagate(void){
    if(visited_fwd) return;
    set_visited_fwd();
}

//
//
void cell::backward_propagate(void){

    if(visited_bwd) return;
    set_visited_bwd();

    int num_inputs = inputs->size();
    for(int i=0; i<num_inputs; i++){
        (*inputs)[i]->set_incoming_loss_gradient(f->get_loss_gradient());
    }

}

//
//
void cell::reset_visited(void){
    visited_fwd = false;
    visited_bwd = false;
}

//
//
void cell::set_visited_fwd(void){
    visited_fwd = true;
}

//
//
void cell::set_visited_bwd(void){
    visited_bwd = true;
}

//
//
output_pin* cell::get_f_pin(void){
    return f;
}

//
//
void cell::set_f_pin(output_pin* f_pin){
    f = f_pin;
    outputs->push_back(f_pin);
}

//
//
vector<input_pin*>* cell::get_inputs(void){
    return inputs;
}

//
//
vector<output_pin*>* cell::get_outputs(void){
    return outputs;
}

//
//
string cell::get_circ_name(void){
    return circ_name;
}

//
//
cell_type cell::get_type(void){
    return type;
}

//
//
bool cell::is_visited_fwd(void){
    return visited_fwd;
}

//
//
bool cell::is_visited_bwd(void){
    return visited_bwd;
}

//
//
float cell::get_loss_gradient(void){
    return f->get_loss_gradient();
}


////////////////////////////////////////////////////////////////////////////
// CONST CELL
////////////////////////////////////////////////////////////////////////////

//
//
void const_cell::forward_propagate(void){
    // This function intentionally left blank
}

//
//
void const_cell::backward_propagate(void){
    // This function intentionally left blank
}

//
//
void const_cell::update_var(void){
    // This function intentionally left blank
}


////////////////////////////////////////////////////////////////////////////
// RAND CELL
////////////////////////////////////////////////////////////////////////////

//
//
void rand_cell::forward_propagate(void){
    // NOTE: get_value() causes value to be updated for dynamic pins, which is
    //       how the output_pin of a rand_cell is configured.
    f->get_value();
}

//
//
void rand_cell::backward_propagate(void){
    // This function intentionally left blank
}

//
//
void rand_cell::update_var(void){
    // This function intentionally left blank
}


////////////////////////////////////////////////////////////////////////////
// VAR CELL
////////////////////////////////////////////////////////////////////////////

//
//
void var_cell::set_var(float x){
    f->set_value(x);
}

//
//
void var_cell::connect_f(input_pin* pin){
// FIXME:
//    f->add_connection_to_pin(pin);
}

//
//
void var_cell::forward_propagate(void){
    // This function intentionally left blank
}

//
//
void var_cell::backward_propagate(void){
// FIXME
}

//
//
void var_cell::update_var(void){

    float f_pin_value = f->get_value();
    float f_pin_gradient = f->get_loss_gradient();
    float new_value = f_pin_value - (10*f_pin_gradient);
    f->set_value(new_value);
//_df(f_pin_value);
//_df(f_pin_gradient);
//_df(new_value);

}


////////////////////////////////////////////////////////////////////////////
// ACT CELL
////////////////////////////////////////////////////////////////////////////

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

//
//
input_pin* act_cell::get_x_pin(void){
    return x;
}

//
//
void act_cell::set_x_pin(input_pin* x_pin){
    x = x_pin;
    inputs->push_back(x_pin);
}

//
//
void act_cell::connect_x(output_pin* pin){
    x->connect_to_pin(pin);
}

//
//
void act_cell::connect_f(input_pin* pin){
    f->add_connection_to_pin(pin);
}

//
//
void act_cell::forward_propagate(void){
    f->set_value(act_fn(x->get_value()));
}

//
//
void act_cell::backward_propagate(void){
}


////////////////////////////////////////////////////////////////////////////
// SOFT MUX CELL
////////////////////////////////////////////////////////////////////////////

//
//
input_pin* soft_mux_cell::get_s_pin(void){
    return s;
}

//
//
void soft_mux_cell::set_s_pin(input_pin* s_pin){
    s = s_pin;
    inputs->push_back(s_pin);
}

//
//
input_pin* soft_mux_cell::get_x0_pin(void){
    return x0;
}

//
//
void soft_mux_cell::set_x0_pin(input_pin* x0_pin){
    x0 = x0_pin;
    inputs->push_back(x0_pin);
}

//
//
input_pin* soft_mux_cell::get_x1_pin(void){
    return x1;
}

//
//
void soft_mux_cell::set_x1_pin(input_pin* x1_pin){
    x1 = x1_pin;
    inputs->push_back(x1_pin);
}

//
//
void soft_mux_cell::forward_propagate(void){

//    float curr_s_pin = s->get_value()
//    float curr_x0_pin = x0->get_value()
//    float curr_x1_pin = x1->get_value()
//
//    float new_f_pin =
//           2 * curr_s_pin * curr_x1_pin
//           +
//          (1-curr_s_pin) * curr_x0_pin;
//
//    new_f_pin = 2*new_f_pin - 1;

//    float new_f = 2 * (t*y1 + (1-t)*y0) - 1;

    float t  = (s->get_value()  + 1) / 2;
    float y0 = (x0->get_value() + 1) / 2;
    float y1 = (x1->get_value() + 1) / 2;

    f->set_value(2 * (t*y1 + (1-t)*y0) - 1);

}

//
//
void soft_mux_cell::backward_propagate(void){
}


////////////////////////////////////////////////////////////////////////////
// LOSS CELL
////////////////////////////////////////////////////////////////////////////

//
//
void loss_cell::connect_x(output_pin* pin, float target){
    loss_pin_target* pt = new loss_pin_target;
    pt->pin = pin;
    pt->target = target;
    connections->push_back(pt);
}

//
//
float loss_cell::get_total_loss(void){
    calculate_total_loss();
    return total_loss;
}

//
//
void loss_cell::calculate_total_loss(void){

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
//_df(total_loss);
    x->set_loss_gradient(total_loss);

}

//
//
void loss_cell::forward_propagate(void){}

//
//
void loss_cell::backward_propagate(void){}

//
//
input_pin* loss_cell::get_x_pin(void){
    return x;
}

//
//
float loss_cell::get_loss_gradient(void){
    return x->get_loss_gradient();
}


// Clayton Bauman 2018

