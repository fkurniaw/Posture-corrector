classdef Score < handle %allows pass by reference
    properties
        total = 0;
    end
    
methods
    function addLittle(score)
        score.total = score.total + 1;
    end
    
    function addBig(score)
        score.total = score.total +5;
    end
    % %i is type integer, and \n adds line break
    function show(score)
        fprintf('SCORE: %i\n', score.total)
    end 
    
end

end 