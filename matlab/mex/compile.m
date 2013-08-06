mex -I../../core/include -c utils.cpp

mex ds_open.cpp   -I../../core/include  utils.o  -L../../lib -lds 
mex ds_close.cpp  -I../../core/include  utils.o  -L../../lib -lds 
mex ds_info.cpp   -I../../core/include  utils.o  -L../../lib -lds 
mex ds_trunc.cpp  -I../../core/include  utils.o  -L../../lib -lds 
mex ds_remove.cpp -I../../core/include  utils.o  -L../../lib -lds 
mex ds_add.cpp    -I../../core/include  utils.o  -L../../lib -lds 
mex ds_write.cpp  -I../../core/include  utils.o  -L../../lib -lds 
mex ds_flush.cpp  -I../../core/include  utils.o  -L../../lib -lds 
mex ds_read.cpp   -I../../core/include  utils.o  -L../../lib -lds 
mex ds_read_index.cpp  -I../../core/include  utils.o  -L../../lib -lds 

