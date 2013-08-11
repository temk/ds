classdef DataStorage < handle
    %classdef DataStorage
    %Example:
    %   ds = DataStorage('/tmp/storage'); % open dataset with mode = 'rwc'
    %
    %   ds.add('uint32',  'Name', 'A');   % add column 'A'
    %   ds.add('double', 'Name', 'B');    % add column 'B'
    %   ds.add('char',    'Name', 'C');   % add column 'C'
    %
    %   % append data to all three columns. Two of then referenced by name   and one by index.
    %   ds.append({'C', 2, 'A'}, {repmat({'one', 'two', 'three', 'four', 'five' }, 1, 10)', 0:0.1:5, [1:100]});
    %
    %   ds.close(); % close object
    %   ============================================================
    %    ds = DataStorage('/tmp/storage', 'r'); %open datatset to read only
    %
    %    FULL = ds.read({'A', 'B', 'C'});                   % read all data to one cell array
    %    full = ds.read({'A', 'B', 'C'}, 'Rows', [1:2:10]); % read partial data to one cell array
    %
    %    [A, B, C] = ds.read({'A', 'B', 'C'});                   % read all data to three variables
    %    [a, b, c] = ds.read({'A', 'B', 'C'}, 'Rows', [1:2:10]); % read partial data to three variables
    %
    %    ds.close(); % close object
    %
    
    properties(Access=private)
        handle = [];
        column = [];
    end
    
    methods(Access=public)
        function [self] = DataStorage(path, varargin)
        %function [self] = DataStorage(path, varargin)
        % Construct new data storage object
        % Parameters:
        %   Mode  - open mode. default is 'rwc', possible values are
        %           following:
        %               r - read
        %               w - write
        %               c - create
        %               t - truncate
        %   Buffer - size of caching buffer. default is 1024 elements
        %
        %Examples:
        %   ds = DataStorage('/tmp/ds1');  % open in RW mode. create if not exists
        %   ds = DataStorage('/tmp/ds2');  % open in RO mode. 
        %
            c = mfilename('class');
            p = mfilename('fullpath');
            addpath([ p(1:end - numel(c)) 'mex']);            
            
            [mode, buff_siz] = get_parameters(self, {'Mode', 'rwc', 'Buffer', 1024}, varargin);
            self.handle = ds_open(path, mode, uint64(buff_siz));
            self.column = ds_info(self.handle);
        end

        function close(self)
        %function close(self)
        %   Close object and release resources
        %
        %Example:
        %   ds.close();
        %    
            ds_close(self.handle);
        end
        
        function flush(self)
        %function flush(self)
        %   Flush datat to disk
        %
        %Example:
        %   ds.flush();
        %    
            ds_flush(self.handle);
        end
        
        function  [I] = info(self)
        %function  [I] = info(self)
        %   Get list of columns
        %
        %Example:
        %   I = ds.info();
        %    
            
            self.column = ds_info(self.handle);
            I = self.column;
        end

        function add(self, type, varargin)
        %function add(self, type, varargin)
        %   Add new column. type can be any matlab type or char16, char32
        %   Accepts following parameters:
        %       Name   - name of column (default - 'var')
        %       Endian - endianness of column. possible values are 'host', 'big' and 'little'. default: 'host'
        %       Index  - adds column to specific index.
        %       EncSize - encoding size. valid only for strings. 
        %
        %Example:
        %   ds.add('uint8', 'Name', 'A'); % create uint8 column named 'A'
        %
        %   b = random(25, 'single');
        %   ds.add( class(b), 'Name', 'b', 'Endian', 'little'); % create float32 column named 'b', little endian
        %    
            [name, endian, index, enc_type] = get_parameters(self, ...
                                                           {'Name', 'var', 'Endian', 'host', 'Index', 0, 'EncType', 'uint32'}, ...
                                                           varargin);
            ds_add(self.handle, type, name, endian, int64(index - 1), enc_type);
            self.column = ds_info(self.handle);
        end
        
        function append(self, cols, data)
        %function append(self, cols, data)
        %   Append data to column
        %       cols - references to columns, may be names or indexes.
        %       data - data to add
        %
        %Example:
        %   ds.append('A', 1:100); 
        %   ds.append(1, 1:100); 
        %   ds.append({'A', 'B', 12}, {1:100, 200:205, rand(125, 1)}); 
        %
            
            col_idx = names_to_indexes(self, cols);
            if numel(col_idx) == 1,
                data = {data};
            end
            
            if numel(data) ~= numel(col_idx)
                error('inconsistent input'); % todo: be more verbose 
            end
               
            for k = 1:numel(data)
                type = self.column(col_idx(k)).type;
                if (~strncmp(type, 'char', 4)) 
                    data{k} = cast(data{k}, type);
                end
            end
            ds_write(self.handle, uint64(col_idx - 1), data);
        end
        
        function truncate(self, cols, length)
        %function truncate(self, cols, length)
        %   Truncate column to specified length
        %
        %Examples:
        %   ds.truncate('A'); %truncate column 'A' to 0
        %   ds.truncate({'A', 'B'}, 10); %truncate columns 'A' and 'B' to 10 elements
        %
           
            col_idx = names_to_indexes(self, cols);
            
            if numel(length) == 1
                length = repmat(length, size(col_idx));
            end
            
            ds_trunc(self.handle, uint64(col_idx - 1), uint64(length));
        end
        
        function remove(self, cols)
        %function remove(self, cols)
        %   remove specified columns
        %
        %Examples:
        %   ds.remove('A')
        %   ds.remove({'A','B', 125})
        %   
            col_idx = names_to_indexes(self, cols);
            ds_remove(self.handle, uint64(col_idx - 1));
        end
        
        
        function [varargout] = read(self, cols, varargin)
        %function [varargout] = read(self, cols, varargin)
        %   read columns data. from specified offset or partially
        %   Accept parameters:
        %       Offset - read from particular offset  
        %       Rows   - read particular rows
        %       Count  - how many rows to read. 
        %   Note: If 'Rows' parameter is present, then 'Offset' and 'Count' parameters are ignored.
        %
        %Examples:
        %   [A, B] = ds.read({'A','B'});
        %   [X]    = ds.read({'A','B'});  % X is cell array, while X{1} is column 'A' and X{2} is column 'B'
        %   [A, B] = ds.read({'A','B'}, 'Rows', 1:2:10); % read 5 rows
        %   [X]     = ds.read({'A','B'}, 'Rows', 1:2:10); % the same, but result saved as cell array
        %
            [row_offs, row_idx, count] = get_parameters(self, {'Offset', 1, 'Rows', [], 'Count', 0}, varargin);
            col_idx = names_to_indexes(self, cols);
            
            if ~isempty(row_idx)
                data = ds_read_index(self.handle, uint64(col_idx - 1), uint64(row_idx - 1));
            else
                data = ds_read(self.handle, uint64(col_idx - 1), uint64(row_offs - 1), uint64(count));
            end
            
            if nargout == 1 && numel(col_idx) > 1
                varargout{1} = data;
            else
                for k = 1:nargout
                    varargout{k} = data{k};
                end
            end
        end
    end
    
    methods(Access = private)
        
        function [varargout] = get_parameters(self, defaults, values)
            s = struct(defaults{:});
            for k = 1:2:numel(values)
                if ~isfield(s, values{k})
                    error(['Unexpected parameter ' values{k}]);
                end
                s.(values{k}) = values{k + 1};
            end
            
            fields = fieldnames(s);
            for k = 1:nargout
                varargout{k} = s.(fields{k});
            end
        end
        
        function [indexes] = names_to_indexes(self, names)
            if ~ischar(names) && ~iscell(names)
                indexes = arg;
                return;
            end
            if ischar(names)
                names = {names};
            end
            
            col_names = {self.column.name};
            
            indexes = zeros(1, numel(names));
            for k = 1:numel(names)
                if ischar(names{k})
                    indexes(k) = find(strcmp(names{k}, col_names));
                else
                    indexes(k) = names{k};
                end
            end
        end
    end
end