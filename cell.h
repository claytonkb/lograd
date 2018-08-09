// cell.h
//

#pragma once

#include "lograd.h"
#include "pin.h"
#include <vector>

using namespace std;

struct loss_pin_target{
    output_pin *pin;
    float       target;
};


                    //////////////////////////////
                    //        CELL (BASE)       //
                    //////////////////////////////

class cell{

    public:
                        cell(string name);
        virtual        ~cell();

        void            reset_visited(void);
        void            set_visited_fwd(void);
        void            set_visited_bwd(void);

        output_pin*     get_f_pin(void);
        void            set_f_pin(output_pin* f_pin);

        vector<input_pin*>*   get_inputs(void);
        vector<output_pin*>*  get_outputs(void);

        string          get_circ_name(void);
        cell_type       get_type(void);

        bool            is_visited_fwd(void);
        bool            is_visited_bwd(void);

        virtual void    forward_propagate(void);
        virtual void    backward_propagate(void);

//        void    forward_propagate(void);
//        void    backward_propagate(void);

        float           get_loss_gradient(void);

    protected:
        cell_type       type;
        output_pin*     f;
        bool            visited_fwd;
        bool            visited_bwd;
        string          circ_name;

        vector<input_pin*>*   inputs;
        vector<output_pin*>*  outputs;

};


                    //////////////////////////////
                    //       SOFT MUX CELL      //
                    //////////////////////////////

class soft_mux_cell : public cell{

    public:
                        soft_mux_cell(string name) : cell(name) {};
                       ~soft_mux_cell()=default;
        input_pin*      get_s_pin(void);
        void            set_s_pin(input_pin* s_pin);
        input_pin*      get_x0_pin(void);
        void            set_x0_pin(input_pin* x_pin);
        input_pin*      get_x1_pin(void);
        void            set_x1_pin(input_pin* x_pin);
        void            forward_propagate(void);
        void            backward_propagate(void);
//        void forward_update(void);
//        void backward_update(void);

    private:
        input_pin*  s;
        input_pin*  x0;
        input_pin*  x1;

};


                    //////////////////////////////
                    //        CONST CELL        //
                    //////////////////////////////

class const_cell : public cell{

    public:
                        const_cell(string name) : cell(name) {};
                       ~const_cell()=default;
        void            set_const(float x);
        void            connect_f(input_pin* pin);
        void            forward_propagate(void);
        void            backward_propagate(void);
        void            update_var(void);

};


                    //////////////////////////////
                    //         RAND CELL        //
                    //////////////////////////////

class rand_cell : public cell{

    public:
                        rand_cell(string name) : cell(name) {};
                       ~rand_cell()=default;
        void            connect_f(input_pin* pin);
        void            forward_propagate(void);
        void            backward_propagate(void);
        void            update_var(void);

};



                    //////////////////////////////
                    //          VAR CELL        //
                    //////////////////////////////

class var_cell : public cell{

    public:
                        var_cell(string name) : cell(name) {};
                       ~var_cell()=default;
        void            set_var(float x);
        void            connect_f(input_pin* pin);
        void            forward_propagate(void);
        void            backward_propagate(void);
        void            update_var(void);

};


                    //////////////////////////////
                    //          ACT CELL        //
                    //////////////////////////////

class act_cell : public cell{

    public:
                        act_cell(string name) : cell(name) {
                            act_fn = &soft_sign;
                        };
                       ~act_cell()=default;

        void            connect_x(output_pin* pin);
        void            connect_f(input_pin*  pin);

        input_pin*      get_x_pin(void);
        void            set_x_pin(input_pin* x_pin);

        void            forward_propagate(void);
        void            backward_propagate(void);

    private:
        input_pin*      x;
        act_fn_ptr      act_fn;

};


                    //////////////////////////////
                    //         LOSS CELL        //
                    //////////////////////////////

class loss_cell : public cell{

    public:
                        loss_cell(string name) : cell(name) {
                            total_loss = 0;
                            connections = new vector<loss_pin_target*>;
                            x = new input_pin;
                            x->set_circ_name("loss.x");
                        };
                       ~loss_cell()=default;
        void            connect_x(output_pin* pin, float target);
//        void            source_loss(void);
        float           get_total_loss(void);
        void            calculate_total_loss(void);
        void            forward_propagate(void);
        void            backward_propagate(void);
        input_pin*      get_x_pin(void);

    private:
        float           total_loss;
        input_pin*      x;
        vector<loss_pin_target*>* connections;

};



// Clayton Bauman 2018

