%upload DataCollect first

%set up serial communication
delete(instrfindall)
arduin = serial('COM5', 'BAUD', 115200);
fclose(arduin)
fopen(arduin)
frequency = 20;

for i=1:5
    fscanf(arduin)
end

fwrite(arduin, 'a')

for i=1:2
    fscanf(arduin)
end

figure
hold on
xlabel('Time')
ylabel('Angle (degrees)')

for i=1/frequency:1/frequency:1000  %divide everything by frequency to get real-time plots; graph is based on displaying 100 points
    
    if i>1/frequency
        delete(htext);
    end % remove previous text overlay
    
    hold on,
    angle = str2double(strcat(fscanf(arduin)))*180/pi; %concatenate char array into string, and convert string to double
    plot(i, angle,'*')
    
    if i<100/frequency % for displaying first 100 points
        axis([0 100/frequency -100 100])
        htext = text(20/frequency, 100,num2str(angle),'HorizontalAlignment','center');
    else
        axis([i-100/frequency i -100 100]) % shift graph
        htext = text((i-80/frequency), 100, num2str(angle),'HorizontalAlignment','center');
    end
    pause(0.0000000001);
end
