%upload DataCollect first

%reset up serial communication
delete(instrfindall)
bluetoot = Bluetooth('HC-06',1);
fclose(bluetoot);
fopen(bluetoot);
frequency = 20;

for i=1:5
    fscanf(bluetoot)
end

fwrite(bluetoot, 'a')

for i=1:2
    fscanf(bluetoot)
end

figure
hold on
xlabel('x (cm)')
ylabel('y (cm)')

axis([-1 15 -1 15]);

angle;

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
        angle = checkForDouble;
    end
    
    P2 = length*[cos(angle/180*pi), sin(angle/180*pi)];
    
    if angle>80
        posture = 'good posture';
    else if angle <75 && angle > 50
            posture = 'fair posture';
        else
            posture = 'poor posture';
        end
    end
    
    crank1 = line([vertex(1), P2(1)], [vertex(2), P2(2)]);    
    htext = text(3, 14,num2str(angle),'HorizontalAlignment','center');
    htext2 = text(7, 14, posture, 'HorizontalAlignment','center');
    
    pause(0.0000001);
    delete(crank1);
end
