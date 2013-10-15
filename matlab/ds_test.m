ds = DataStorage('/tmp/ds', 'Mode', 'rwct');

ds.add('uint8', 'Name', 'A', 'Width', 4);
I = ds.info(); 
X = reshape(1:20, 4, [])';
ds.append('A', X);
ds.flush();

A = ds.read('A');
B = ds.read('A', 'Rows', 1:2:5);

ds.truncate('A', 3);
a = ds.read('A');
ds.close();