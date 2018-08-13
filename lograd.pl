#!/usr/bin/perl

use strict;
use Data::Dumper;

my $guid=0x80000000;

print <<'END_QUOTE';
-- input cell declaration section --
.cel

.var
s0 1
s1 0

-- Note: initialization can be 0, 1 or r --

-- constant cell declaration section --
.con
c0 -1
c1 1
c2 1
c3 -1

-- random cell declaration section --
-- .ran --
-- r0 1 --

-- activation cells declaration section --
.act
a0
a1

-- soft-mux cells declaration section --
.mux
m0
m1
m2

-- wires connection section --
.wir
s0.f a0.x
s1.f a1.x

a0.f m0.s
a0.f m1.s
a1.f m2.s

-- r0.f m0.x0 --
c0.f m0.x0
c1.f m0.x1
c2.f m1.x0
c3.f m1.x1

m0.f m2.x0
m1.f m2.x1

.out
m2

-- loss section --
.los

m2.f 1
END_QUOTE


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
#
sub map_declare{ # declare input variables

    my $wires = shift;
    for(@{$wires}){
        print "$_;\n";
    }

}


#
#
sub map_assign_eq{

    my $left = shift;
    my $right = shift;
    my $length = ($#{$left} < $#{$right}) ? $#{$left} : $#{$right};

    for(my $i=0; $i<=$length; $i++){
        my $left_wire = shift @{$left};
        my $right_wire = shift @{$right};
        print "ASSIGN EQUIV($left_wire, $right_wire);\n";
        
    }

}


#
#
sub expand_constant{ # takes a constant (up to 32 bits wide) and expands it to [TF] literal bus

    my $width = shift;
    my $const = hex(shift);
    my $result;

    for(my $i=0; $i<$width; $i++){
        my $mask = (1<<$i);
        if($const & $mask){
            push @{$result}, "T";
        }
        else{
            push @{$result}, "F";
        }
    }

    return $result;

}


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





