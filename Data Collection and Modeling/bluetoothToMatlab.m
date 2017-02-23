%upload DataCollect first

%restet up serial communication
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
xlabel('Time')
ylabel('Angle (degrees)')

angle;

for i=1:100000  %divide everything by frequency to get real-time plots; graph is based on displaying 100 points
    
    if i>1
        delete(htext);
    end % remove previous text overlay
    
    hold on,
    
%     if(i<100/frequency)
%         angle = [angle str2double(strcat(fscanf(bluetoot)))*180/pi]; %concatenate char array into string, and convert string to double
%         plot(i, angle(length(angle)),'*')
%     else
%         delete(angle(1));
%         angle = [angle(2):1:angle(length(angle)-1)];
%         angle(100) = str2double(strcat(fscanf(bluetoot)))*180/pi; 
%         plot(i, angle(100),'*')
%     end        
    
    
    checkForDouble = str2double(strcat(fscanf(bluetoot)))*180/pi;
    if isnan(checkForDouble)
        i = i-1;
        return;
    else
        angle = checkForDouble;
    end
    
    x = plot(i/frequency, angle, '*');
    
    if i<100 % for displaying first 100 points
        axis([0 100/frequency -100 100])
        htext = text(20/frequency, 100,num2str(angle),'HorizontalAlignment','center');
    else
        axis([(i-100)/frequency i/frequency -100 100]) % shift graph
        htext = text((i-80)/frequency, 100, num2str(angle),'HorizontalAlignment','center');
    end
    pause(0.0000000001);
end