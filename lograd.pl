#!/usr/bin/perl

use strict;
use Data::Dumper;

# TODO

# In the original script, we just spit out equations and let the rest of the
# toolchain worry about what to do with them. In this script, we have to
# keep track of the following:
#
#   table of variables
#   table of constants
#   table of activation cells
#   table of mux cells
#   table of wires
#   table of outputs
#   generate loss section
# 
# ... then once we have processed the entire design, we will emit it to file.
# Should probable create some kind of struct/tuple to hold everything.

# m2 = XOR(s0,s1) would produce the following code:

my $guid=0x1111;

my $var_tab = {};
my $con_tab = [];
my $ran_tab = [];
my $act_tab = [];
my $mux_tab = [];
my $wir_tab = [];
my $out_tab = [];
my $los_tab = [];

$var_tab->{"act"} = {};
$var_tab->{"ini"} = {};

my $fn = {
   "false" =>  0,
   "and"   =>  4,
   "in1"   => 12,
   "in0"   => 20,
   "xor"   => 24,
   "or"    => 28,
   "nor"   => 32,
   "eq"    => 36,
   "xnor"  => 36,
   "!in0"  => 40,
   "!in1"  => 44,
   "nand"  => 56,
   "true"  => 60,
};

my $tot = { # tot = table-of-tables
    ".guid" => $guid,
    ".var" => $var_tab,
    ".con" => $con_tab,
    ".ran" => $ran_tab,
    ".act" => $act_tab,
    ".mux" => $mux_tab,
    ".wir" => $wir_tab,
    ".out" => $out_tab,
    ".los" => $los_tab,
};

init_con($tot);

my $act0 = insert_var($tot,"s0","l2");
my $act1 = insert_var($tot,"s1","l2");
#my $out = insert_fn2($tot,$fn->{"xor"},"s0","s1");
my $out = insert_fn2($tot,$fn->{"and"},$act0,$act1);
designate_outputs($tot,$out);
push @{$tot->{".los"}}, "$out.f 1";
emit_lgc($tot);

#my $foo = expand_constant(32,"deadbeef");
#print Dumper($foo);

die;


sub insert_var{

    my ($tot, $name, $init) = @_;

    my $var_tab = $tot->{".var"};
    my $act_tab = $tot->{".act"};
    my $wir_tab = $tot->{".wir"};

    my $act = "act$tot->{'.guid'}"; $tot->{".guid"}+=1;

    $var_tab->{"act"}{$name} = $act;
    $var_tab->{"ini"}{$name} = $init;

    push @{$act_tab}, $act;
    push @{$wir_tab}, "$name.f $act.x";

    return $act;

}


sub init_con{

    my $tot = shift;

    push @{ $tot->{".con"} },
          ("c00 -1", "c01 -1", "c02 -1", "c03 -1",    #  0  0  0  0  FALSE
           "c04 -1", "c05 -1", "c06 -1", "c07  1",    #  0  0  0  1  AND
           "c08 -1", "c09 -1", "c10  1", "c11 -1",    #  0  0  1  0
           "c12 -1", "c13 -1", "c14  1", "c15  1",    #  0  0  1  1  in1
           "c16 -1", "c17  1", "c18 -1", "c19 -1",    #  0  1  0  0
           "c20 -1", "c21  1", "c22 -1", "c23  1",    #  0  1  0  1  in0
           "c24 -1", "c25  1", "c26  1", "c27 -1",    #  0  1  1  0  XOR
           "c28 -1", "c29  1", "c30  1", "c31  1",    #  0  1  1  1  OR
           "c32  1", "c33 -1", "c34 -1", "c35 -1",    #  1  0  0  0  NOR
           "c36  1", "c37 -1", "c38 -1", "c39  1",    #  1  0  0  1  EQ
           "c40  1", "c41 -1", "c42  1", "c43 -1",    #  1  0  1  0  !in0
           "c44  1", "c45 -1", "c46  1", "c47  1",    #  1  0  1  1
           "c48  1", "c49  1", "c50 -1", "c51 -1",    #  1  1  0  0  !in1
           "c52  1", "c53  1", "c54 -1", "c55  1",    #  1  1  0  1
           "c56  1", "c57  1", "c58  1", "c59 -1",    #  1  1  1  0  NAND
           "c60  1", "c61  1", "c62  1", "c63  1");   #  1  1  1  1  TRUE

}


# takes a constant (up to 32 bits wide) and expands it to [-1,1] literal bus
#
sub expand_constant{

    my $width = shift;
    my $const = hex(shift);
    my $result;

    for(my $i=0; $i<$width; $i++){
        my $mask = (1<<$i);
        if($const & $mask){
            push @{$result}, 1;
        }
        else{
            push @{$result}, -1;
        }
    }

    return $result;

}


#map_pins("s", ".f", [0..31], "a", ".x", [0..31])
sub map_pins{

    my ($tot, $src_cell,  $src_pin,   $src_indices,
              $dest_cell, $dest_pin,  $dest_indices) = @_;

    my $wir_tab = $tot->{".wir"};

    for(0..$#{$dest_indices}){
        push @{$wir_tab}, 
            "${src_cell}$src_indices->[$_].$src_pin ${dest_cell}$dest_indices->[$_].$dest_pin";
    }

}


#       map_mux("m0", [0..31],
#               "s", ".f", [0..31], 
#               "A", ".f", [0..31],
#               "B", ".f", [0..31]);
#
sub map_mux{

    my ($tot, 
        $mux_cell, $mux_indices,
        $s_cell,  $s_pin,  $s_indices,
        $x0_cell, $x0_pin, $x0_indices,
        $x1_cell, $x1_pin, $x1_indices) = @_;

    my $wir_tab = $tot->{".wir"};

    map_pins($tot, $s_cell,   $s_pin, $s_indices,
                   $mux_cell, "s",    $mux_indices);

    map_pins($tot, $x0_cell,  $x0_pin, $x0_indices,
                   $mux_cell, "x0",    $mux_indices);

    map_pins($tot, $x1_cell,  $x1_pin, $x1_indices,
                   $mux_cell, "x1",    $mux_indices);

}


sub insert_mux{

    my ($tot, $s_in, $x0_in, $x1_in) = @_;
    my $mux = "mux$tot->{'.guid'}"; $tot->{".guid"}++;

    my $wir_tab = $tot->{".wir"};

    push @{$wir_tab},
       ("$s_in  $mux.s",
        "$x0_in $mux.x0",
        "$x1_in $mux.x1");

    return $mux;

}


sub insert_fn2{

    my ($tot, $fn, $in0, $in1) = @_;

    my $m0 = "mux$tot->{'.guid'}"; $tot->{".guid"}++;
    my $m1 = "mux$tot->{'.guid'}"; $tot->{".guid"}++;
    my $m2 = "mux$tot->{'.guid'}"; $tot->{".guid"}++;

    my $mux_tab = $tot->{".mux"};
    my $wir_tab = $tot->{".wir"};

    push @{$mux_tab}, ($m0, $m1, $m2);

    push @{$wir_tab},
       ("$in0.f $m0.s",
        "$in0.f $m1.s",
        "$in1.f $m2.s");

    push @{$wir_tab},
        "c$fn.f $m0.x0"; $fn++;

    push @{$wir_tab},
        "c$fn.f $m0.x1"; $fn++;

    push @{$wir_tab},
        "c$fn.f $m1.x0"; $fn++;

    push @{$wir_tab},
        "c$fn.f $m1.x1";

    push @{$wir_tab},
       ("$m0.f $m2.x0",
        "$m1.f $m2.x1");

    return $m2; #name of the output cell

}

sub designate_outputs{
    $tot = shift;
    push @{$tot->{".out"}}, @_;
}

sub emit_lgc{

    my $tot = shift;

    $var_tab = $tot->{".var"};
    $con_tab = $tot->{".con"};
    $ran_tab = $tot->{".ran"};
    $act_tab = $tot->{".act"};
    $mux_tab = $tot->{".mux"};
    $wir_tab = $tot->{".wir"};
    $out_tab = $tot->{".out"};
    $los_tab = $tot->{".los"};

    print ".cel\n";

    print ".var\n";
    print "$_ $var_tab->{'ini'}{$_}\n" for (keys %{$var_tab->{"ini"}});
    print "\n";

    print ".con\n";
    print "$_\n" for(@{$con_tab});
    print "\n";

    print ".ran\n";
    print "$_\n" for(@{$ran_tab});
    print "\n";

    print ".act\n";
    print "$_\n" for(@{$act_tab});
    print "\n";

    print ".mux\n";
    print "$_\n" for(@{$mux_tab});
    print "\n";

    print ".wir\n";
    print "$_\n" for(@{$wir_tab});
    print "\n";

    print ".out\n";
    print "$_\n" for(@{$out_tab});
    print "\n";

    print ".los\n";
    print "$_\n" for(@{$los_tab});
    print "\n";

#    foreach my $section ( qw{ .var .con .ran .act .mux .wir .out .los } ){
#        print "$section\n";
#        print "$_\n" for(@{$tot->{$section}});
#        print "\n";
#    }

}

#my $k = [ qw( stuff ) ];
#
#my $i;
#for($i=0; $i<64; $i++){
#    map_wires(expand_bus("i$i.k", [0..31]), expand_constant(32,$k->[$i]));
#}
#
#for($i=0; $i<16; $i++){
#    map_wires(expand_bus("i$i.w", [0..31]), expand_constant(32,$w->[$i]));
#}
#
##for($i=8; $i<16; $i++){
##    map_wires(expand_bus("i$i.w", [0..31]), expand_constant(32,$k->[$i-8]));
##}
#
##map_assign_eq(expand_bus("h", [0..7]), [("F")x8]);
#
#    my $xor_temp1 = wire_uniq("i$i");
#
#    my $i2  = $i-2;
#
#    map_fn2("ODD", 
#        expand_bus($xor_temp1, [0..31]), 
#        expand_bus("i${i2}.w", rotate_right_array(19,[0..31])),
#        map_shift_right(10, "i${i2}.w", [0..31]));
#
#    map_add_modulo(
#        "i$i.m0",
#        expand_bus($add_temp1, [0..31]),
#        expand_bus("i${i7}.w", [0..31]),
#        expand_bus($xor_temp2, [0..31]));
#
#    my $xor_temp = wire_uniq("i$i.ep0");
#    map_fn2("EVEN", 
#        expand_bus($xor_temp, [0..31]),
#        expand_bus("${g4}", rotate_right_array(25,[0..31])),
#        expand_bus("${g4}", rotate_right_array(11,[0..31])));
#
#    map_maj(
#        "i$i.maj",
#        expand_bus("i$i.b6", [0..31]),
#        expand_bus("${g0}",  [0..31]),
#        expand_bus("${g1}",  [0..31]),
#        expand_bus("${g2}",  [0..31]));


#
#
sub expand_bus{

    my $base = shift;
    my $indices = shift;
    my $wires;

    for(@{$indices}){
        my $wire = "${base}_$_";
        push @{$wires}, $wire;
    }

    return $wires;

}


#
#
sub wire_uniq{

    my $base = "wire";
    $base = shift if ($#_>=0);
    return sprintf("${base}_%08x", $guid++);

}


#
#
sub map_fn1{ # map single-argument function (NOT)

    my $fn_name = shift;
    my $dest_wires = shift;
    my $src_wires = shift;

    for(@{$dest_wires}){
        my $src_wire = shift @{$src_wires};
        print "$_ := $fn_name($src_wire);\n";
    }

}


#
#
sub map_fn2{ # map two-argument function...

    my $fn_name = shift;
    my $dest_wires = shift;
    my $srcA_wires = shift;
    my $srcB_wires = shift;

    for(@{$dest_wires}){
        my $srcA_wire = shift @{$srcA_wires};
        my $srcB_wire = shift @{$srcB_wires};
        print "$_ := $fn_name($srcA_wire, $srcB_wire);\n";
    }

}


#
#
sub map_fn3{ # map three-argument function (ITE)

    my $fn_name = shift;
    my $dest_wires = shift;
    my $srcA_wires = shift;
    my $srcB_wires = shift;
    my $srcC_wires = shift;

    for(@{$dest_wires}){
        my $srcA_wire = shift @{$srcA_wires};
        my $srcB_wire = shift @{$srcB_wires};
        my $srcC_wire = shift @{$srcC_wires};
        print "$_ := $fn_name($srcA_wire, $srcB_wire, $srcC_wire);\n";
    }

}


#
#
sub map_wires{

    my $dest_wires = shift;
    my $src_wires = shift;

    for(@{$dest_wires}){
        my $src_wire = shift @{$src_wires};
        print "$_ := $src_wire;\n";
    }

}


#
#
sub map_add_modulo{ #FIXME: connect carries, eliminate first cin, last cout

    my $prefix = shift;
    my $sum_wires = shift;
    my $srcA_wires = shift;
    my $srcB_wires = shift;

    my $srcA_wire;
    my $srcB_wire;

    $_ = shift @{$sum_wires};
    $srcA_wire = shift @{$srcA_wires};
    $srcB_wire = shift @{$srcB_wires};
    my $carry_out = wire_uniq($prefix);
    my $carry_in = $carry_out;

    print "$_ := ODD($srcA_wire, $srcB_wire);\n";
    print "$carry_out := AND($srcA_wire, $srcB_wire);\n";

    for(@{$sum_wires}){
        $srcA_wire = shift @{$srcA_wires};
        $srcB_wire = shift @{$srcB_wires};
        my $half_sum = wire_uniq($prefix);
        my $carry_out = wire_uniq($prefix);
        print "$half_sum := ODD($srcA_wire, $srcB_wire);\n";
        print "$_ := ODD($half_sum, $carry_in);\n";
        print "$carry_out "
                . ":= OR("
                . "AND($srcA_wire, $srcB_wire), "
                . "AND($half_sum, $carry_in));\n";
        $carry_in = $carry_out;
    }

}


#
#
sub map_sub_modulo{

    my $prefix = shift;
    my $sum_wires = shift;
    my $srcA_wires = shift;
    my $srcB_wires = shift;

    my $msb = $#{$sum_wires};
    my $comp_srcA = wire_uniq($prefix);
    map_fn1("NOT", expand_bus("$comp_srcA",[0..$msb]), $srcA_wires);

    my $srcA_wire=0;
    my $srcB_wire;

    my $carry_out;
    my $carry_in = "T";

    for(@{$sum_wires}){
        $srcB_wire = shift @{$srcB_wires};
        my $half_sum = wire_uniq($prefix);
        my $carry_out = wire_uniq($prefix);
        print "$half_sum := ODD(${comp_srcA}_$srcA_wire, $srcB_wire);\n";
        print "$_ := ODD($half_sum, $carry_in);\n";
        print "$carry_out "
                . ":= OR("
                . "AND(${comp_srcA}_$srcA_wire, $srcB_wire), "
                . "AND($half_sum, $carry_in));\n";
        $carry_in = $carry_out;
        $srcA_wire++;
    }

}



#
#
sub map_maj{

    my $prefix = shift;
    my $dest_wires = shift;
    my $srcA_wires = shift;
    my $srcB_wires = shift;
    my $srcC_wires = shift;

    for(@{$dest_wires}){
        my $srcA_wire = shift @{$srcA_wires};
        my $srcB_wire = shift @{$srcB_wires};
        my $srcC_wire = shift @{$srcC_wires};
        my $new_wire0 = wire_uniq($prefix);
        my $new_wire1 = wire_uniq($prefix);
        my $new_wire2 = wire_uniq($prefix);
        print "$new_wire0 := AND($srcA_wire, $srcB_wire);\n";
        print "$new_wire1 := AND($srcB_wire, $srcC_wire);\n";
        print "$new_wire2 := AND($srcA_wire, $srcC_wire);\n";
        print "$_ := ODD($new_wire0, $new_wire1, $new_wire2);\n";
    }

}



# Use this to set up an implicit rotate-right
#
sub rotate_right_array{

    my $distance = shift;
    my $indices  = shift;

    my $temp = [ @{$indices} ];

    splice @{$indices}, $distance;
    splice @{$temp}, 0, $distance;

    push @{$temp}, @{$indices};

    return $temp;

}


#
#
sub map_shift_right{ #shifts a bus right-wise and returns expanded wires

    my $distance = shift;
    my $base     = shift;
    my $indices  = shift;
    my $result;

    for(@{$indices}){
        push @{$result}, "${base}_$_";
    }

    splice @{$result}, 0, $distance;

    for(1..$distance){
        push @{$result}, "F";
    }

    return $result;

}



## mulex.pl
#
#use strict;
#
##static inline uint32_t rotl(const uint32_t x, int k) {
##	return (x << k) | (x >> (32 - k));
##}
#
#
##static uint32_t s[4];
#
#my $time = time();
#print dec2hex($time) . "\n";
#
##our $s = [($time^0xdeadbeef)<<32,$time^0xfeedcafedeaffade];
#our $s = [$time+0xdeadbeef,$time+0xbabeface];
#
##^0x0123456789abcdef
##^0xfedcba9876543210
##print dec2hex( rotl( 0xdeadbeefbabeface, 4 ) );
#
#xoroshiro128() for (1..10);
##print dec2hex( xoroshiro128() ) . "\n";
#print dec2hex( $s->[0] ) . "\n";
#
#sub dec2hex { return sprintf("%x",shift) }
#
#sub xoroshiro128{
#
#    our $s;
#
#    my $s0 = $s->[0];
#    my $s1 = $s->[1];
#
#    my $result = $s0+$s1;
#
#	$s1 ^= $s0;
#	$s->[0] = rotl($s0, 24) ^ $s1 ^ ($s1 << 16);
#	$s->[1] = rotl($s1, 37);
#
##	return $result;
#
#}
#
#sub rotl{
#    my ($x, $k) = @_;
#    return ($x << $k) | ($x >> (64 - $k));
#}







