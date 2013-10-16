mex -I../../core/include -I../include -c utils.cpp

mex ds_open.cpp   -I../../core/include  -I../include utils.o -L/home/boris/lib -lds 
mex ds_close.cpp  -I../../core/include  -I../include utils.o -L/home/boris/lib -lds 
mex ds_info.cpp   -I../../core/include  -I../include utils.o -L/home/boris/lib -lds 
mex ds_trunc.cpp  -I../../core/include  -I../include utils.o -L/home/boris/lib -lds 
mex ds_remove.cpp -I../../core/include  -I../include utils.o -L/home/boris/lib -lds 
mex ds_add.cpp    -I../../core/include  -I../include utils.o -L/home/boris/lib -lds 
mex ds_write.cpp  -I../../core/include  -I../include utils.o -L/home/boris/lib -lds 
mex ds_flush.cpp  -I../../core/include  -I../include utils.o -L/home/boris/lib -lds 
mex ds_read.cpp   -I../../core/include  -I../include utils.o -L/home/boris/lib -lds 
mex ds_meta_add.cpp   -I../../core/include  -I../include utils.o -L/home/boris/lib -lds 
mex ds_meta_get.cpp   -I../../core/include  -I../include utils.o -L/home/boris/lib -lds 
mex ds_read_index.cpp  -I../../core/include  -I../include utils.o -L/home/boris/lib -lds 

