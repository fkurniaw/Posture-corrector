%upload DataCollect first

%reset up serial communication
% delete(instrfindall)
% % bluetoot = Bluetooth('HC-06',1);
% fclose(bluetoot);
% fopen(bluetoot);
frequency = 20;

for i=1:5
    fscanf(bluetoot)
end

fwrite(bluetoot, 'a')

for i=1:4
    fscanf(bluetoot)
end

figure
hold on
xlabel('x (cm)')
ylabel('y (cm)')

axis([-1 15 -1 15]);

length = 12;
vertex = [0 0];

for i=1:100000  %divide everything by frequency to get real-time plots; graph is based on displaying 100 points
    
    if i>1
        delete(htext);
        delete(htext2);
    end % remove previous text overlay
    
    hold on,
  
    checkForDouble = str2double(strcat(fscanf(bluetoot)))*180/pi;
    if isnan(checkForDouble)
        i = i-1;
        return;
    else
        angleBreadBoard = checkForDouble;
    end
    
    P2 = length*[cos(angleBreadBoard/180*pi), sin(angleBreadBoard/180*pi)];
    
    if angleBreadBoard>75
        posture = 'good posture';
    else if angleBreadBoard <75 && angleBreadBoard > 50
            posture = 'fair posture';
        else
            posture = 'poor posture';
        end
    end
    
    crank1 = line([vertex(1), P2(1)], [vertex(2), P2(2)]);    
    htext = text(3, 14,num2str(angleBreadBoard),'HorizontalAlignment','center');
    htext2 = text(7, 14, posture, 'HorizontalAlignment','center');
    
    pause(0.0000000001);
    delete(crank1);
end
