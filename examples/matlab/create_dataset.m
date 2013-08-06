function [ds] = create_dataset(col_num, row_num, str_frac)
    types = {'double', 'single', 'int32', 'int16', 'int8', 'logical'};
    
    type_idx = randi([1, numel(types)], 1, col_num);
    col_types = types(type_idx);
    
    str_mask = rand(1, col_num) < str_frac;
    col_types(str_mask) = {'char'};
    
    ds = dataset();
    for k = 1:col_num
        field = sprintf('var_%d', k);
        type  = col_types{k};
        
        switch type
            case {'double', 'single'}
                value = rand(row_num, 1, type);
                
                % set 10 % of Inf and 10% of NaN
                inf_mask = rand(row_num, 1) < 0.1; 
                value(inf_mask) = Inf;
                
                nan_mask = rand(row_num, 1) < 0.1;
                value(nan_mask) = NaN;
                
                ds.(field) = value;
                
            case {'int32', 'int16', 'int8'}
                ds.(field) = randi(intmax(type), [row_num, 1],type);
        
            case {'logical'}
                ds.(field) = rand(row_num, 1) < 0.5;
                
            case {'char'}
                ds.(field) = rand_str(row_num);
                
            otherwise
                error(['Unexpected type:' col_types{k}]);
        end
    end
end
function [col] = rand_str(num)
    persistent words;
    if isempty(words)
        disp('Initialize words');
        symbols = ['a':'z' 'A':'Z' '0':'9'];
        MIN_LEN = 3;
        MAX_LEN = 10;
        WORD_NUM = 1000000;
        words = cell(WORD_NUM, 1);
        for i = 1:numel(words)
            len = randi([MIN_LEN, MAX_LEN]);
            idx = randi(numel(symbols), [1, len]);
            words{i} = symbols(idx);
        end
    end
    
    col = words(randi([numel(words)], [num, 1]));
end
