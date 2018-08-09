// pin.h
//

#pragma once

#include "lograd.h"
#include "numeric.h"
#include <vector>

using namespace std;

class cell; // deferred type-declarations
class input_pin;
class output_pin;

                    //////////////////////////////
                    //        PIN (BASE)        //
                    //////////////////////////////

class pin{

    public:
                        pin();
        virtual        ~pin()=default;

        void            init_value(pin_init pi);
        virtual float   get_value(void);
        void            set_value(float x);

        float           get_loss_gradient(void);
        void            set_loss_gradient(float x);
        void            accumulate_loss_gradient(float x);

        void            reset_pin_state(void);

        cell*           get_this_cell(void);
        void            set_this_cell(cell* c);

        pin_type        get_type(void);

        string          get_circ_name(void);
        void            set_circ_name(string s);

        virtual void    forward_update(void);
        virtual void    backward_update(void);

        void            reset_pin_visited(void);

    protected:
        bool            visited_fwd;
        bool            visited_bwd;

        string          circ_name;

        pin_init        init;
        pin_state       state;
        pin_type        type;

        float           value;
        float           loss_gradient;

        cell*           this_cell;

};



                    //////////////////////////////
                    //        OUTPUT PIN        //
                    //////////////////////////////

class output_pin : public pin{

    public:
                        output_pin();
                        output_pin(pin_init pi);
                        output_pin(pin_init pi, bool dynamic);
                       ~output_pin()=default;
        void            source_value(void);         // push value to connected pins
        float           get_value(void);            // overrides the base class method
        void            sink_loss_gradient(void);   // pull loss_grad from connected pins
        void            add_connection_to_pin(input_pin* pin);
        void            forward_update(void);
        void            backward_update(void);

    private:
        vector<input_pin*>* connections;
        bool dynamic_pin;

};


                    //////////////////////////////
                    //         INPUT PIN        //
                    //////////////////////////////

class input_pin : public pin{

    public:
                        input_pin();
                       ~input_pin()=default;
        bool            connect_to_pin(output_pin* pin);
        void            sink_value(void);           // pull value from connected pin
        void            source_loss_gradient(void); // push loss_grad to connected pin
        bool            is_connected(void);
        output_pin*     get_connection(void);
        void            forward_update(void);
        void            backward_update(void);
        void            set_incoming_loss_gradient(float x);

    private:
        float           incoming_loss_gradient;
        bool            connected;
        output_pin*     connection;

};


// Clayton Bauman 2018

