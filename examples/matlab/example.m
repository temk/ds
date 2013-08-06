addpath ../../matlab

ds = DataStorage('/tmp/storage');

ds.add('uint32',  'Name', 'A');
ds.add('double', 'Name', 'B');
ds.add('char',    'Name', 'C');

ds.append({'C', 2, 'A'}, {repmat({'one', 'two', 'three', 'four', 'five' }, 1, 10)', 0:0.1:5, [1:100]});
ds.close();
%%
ds = DataStorage('/tmp/storage');

FULL = ds.read({'A', 'B', 'C'});
full = ds.read({'A', 'B', 'C'}, 'Rows', [1:2:10]);

[A, B, C] = ds.read({'A', 'B', 'C'});
[a, b, c] = ds.read({'A', 'B', 'C'}, 'Rows', [1:2:10]);

ds.close();
%%
ds = DataStorage('/tmp/storage');
ds.add('char', 'Name', 'dripsa');
ds.append('dripsa', {'yes', 'no', 'may be'});
ds.flush();
ds.close();
%%
ds = DataStorage('/tmp/storage');

ds.add('char',    'Name', 'A');
ds.add('double', 'Name', 'B');

B = rand(100000000, 1);
A = repmat({'one', 'two', 'three', 'four', 'five' }, 1, 20000000)';

tic;
ds.append('B', B);
toc;
%%