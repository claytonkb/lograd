// circuit.h

#pragma once

#include "lograd.h"
#include "cell.h"
#include "pin.h"
#include <vector>

using namespace std;


////////////////////////////////////////////////////////////////////////////
// CIRCUIT
////////////////////////////////////////////////////////////////////////////

class circuit{

    public:
                            circuit();
                            circuit(string_vector* lgc);
                           ~circuit()=default;

        void                iterate(int n);
        void                iterate(int n, float g);

        void                do_forward_pass(void);
        void                do_backward_pass(void);

        void                update_loss(void);
        void                update_vars(void);
        void                update_vars(float g);

        void                reset_iteration(void);

        void                init_circ_map(string_vector* lgc);
        void                init_pin_list(void);

        void                add_cell(cell *b, cell_role r, cell_type t);
        void                add_cell(cell *b, cell_role r);

        vector<cell*>*      get_all_cells(void);
        vector<cell*>*      get_output_cells(void);
        vector<var_cell*>*  get_variable_cells(void);

        void                designate_output_cell(cell *b);
        loss_cell*          get_loss_cell(void);

        void                pin_list_insert(pin* p);
        vector<pin*>*       get_pin_list(void);

        void                show_pin_values(void);
        void                show_pin_gradients(void);
        void                show_input_vars(void);

        float               get_circuit_loss(void);

        string_map*         get_circ_map(void);

    private:
        direction           mode_select;  // switch between forward and reverse modes

        vector<cell*>*      all_cells;    // all cells in this circuit
        vector<cell*>*      input_cells;  // set of cells from which we initiate forward_update
        vector<cell*>*      prop_cells;
        vector<cell*>*      output_cells;
        vector<var_cell*>*  variables;    // the set of cells we are solving

        loss_cell*          circuit_loss; // the loss cell (singleton)

        vector<pin*>*       pin_list;

        string_map*         circ_map;

};


// Clayton Bauman 2018

