#!/usr/bin/perl 
#
#

#
# script to generate type safe rw methods for class org.temk.ds.Column
#
use strict;
use warnings;

use Cwd qw(abs_path);

use File::Copy;
use File::Basename;
use File::Temp qw(tempfile);

my $dirname = dirname(__FILE__);

my @types = qw(boolean byte short int long float double String);
my $input = abs_path($dirname . '/../java/org/temk/ds/Column.java');
my ($fd, $output) = tempfile();

open(INPUT, $input) || die "Can't open $input: $!";

while(<INPUT>) 
{
	print $fd $_;
	last if /__DO_NOT_EDIT_GENERATED_CODE__/;
}

while(<INPUT>) 
{
	last if /__END_OF_GENERATED_CODE__/;
}

foreach my $type (@types) {
print $fd <<__END_OF_MARK__;

    /**
     * Appends $type array to column
     * \@param data - data to append 
     * \@return this column 
    **/
    public Column append($type [] data) {
        write(data);
        return this;
    }

    /**
     * Reads nun elements from specified offset into $type array.
     * \@param num - number of elements to read
     * \@param offset - number of elements to skip
     * \@param data - buffer for data, will allocated if it null
     * \@return data that was read
    **/
    public $type [] read(long offset, int num, $type [] data) {
        if (data == null) {
            data = new $type \[num\];
        }
        read(data, null, 0, offset, num);
        return data;
    }

    /**
     * Reads all elements from specified offset into $type array.
     * \@param offset - number of elements to skip
     * \@param data - buffer for data, will allocated if it null
     * \@return data that was read
    **/
    public $type [] read(long offset, $type [] data) {
        return read(offset, (int)(getLength() - offset + 1), data);
    }

    /**
     * Reads all elements from specified offset into $type array.
     * \@param offset - number of elements to skip
     * \@param data - buffer for data, will allocated if it null
     * \@return data that was read
    **/
    public $type [] read($type [] data) {
        return read(0, (int )getLength(), data);
    }

    /**
     * Reads elements specified by index array.
     * \@param indexes - indexes to read
     * \@param data - buffer for data, will allocated if it null
     * \@return data that was read
    **/
    public $type [] read(int [] indexes, $type [] data) {
        if (data == null) {
            data = new $type \[indexes.length\];
        }
        read(data, indexes, Integer.SIZE/Byte.SIZE, 0, 0);

        return data;
    }

    /**
     * Reads elements specified by index array.
     * \@param indexes - indexes to read
     * \@param data - buffer for data, will allocated if it null
     * \@return data that was read
    **/
    public $type [] read(long [] indexes, $type [] data) {
        if (data == null) {
            data = new $type \[indexes.length\];
        }
        read(data, indexes, Long.SIZE/Byte.SIZE, 0, 0);

        return data;
    }

__END_OF_MARK__
}

print $fd '    // __END_OF_GENERATED_CODE__', $/;

print $fd $_ while(<INPUT>);
close(INPUT);
close($fd);

move($output, $input);



