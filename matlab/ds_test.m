ds = DataStorage('/tmp/ds', 'Mode', 'rwct');

ds.add('uint8', 'Name', 'A', 'Width', 4);
I = ds.info(); 
X = reshape(1:20, 4, [])';
ds.append('A', X);

ds.add_meta('opsa1', 'dripsa1', 'A');
ds.add_meta('opsa2', 'dripsa2', 'A');

ds.add_meta('foo1', 'bar1');
ds.add_meta('foo2', 'bar2');

ds.flush();

A = ds.read('A');
B = ds.read('A', 'Rows', 1:2:5);

ds.truncate('A', 3);
a = ds.read('A');

[v, k] = ds.get_meta();
[v, k] = ds.get_meta([]);
[v, k] = ds.get_meta(1);
[v, k] = ds.get_meta('A');

[v] = ds.get_meta([], {'foo2', 'no_key', 'foo1'});
[v] = ds.get_meta([], 'foo2');
[v] = ds.get_meta([], 'foo3');

[v] = ds.get_meta('A', {'opsa2', 'no_key', 'opsa1'});
[v] = ds.get_meta('A', 'opsa2');
[v] = ds.get_meta('A', 'opsa3');

ds.close();