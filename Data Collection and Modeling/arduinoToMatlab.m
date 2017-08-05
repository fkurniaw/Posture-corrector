%upload DataCollect first

%set up serial communication
delete(instrfindall)
arduin = serial('COM6', 'BAUD', 115200);
fclose(arduin)
fopen(arduin)
frequency = 20; 
for i=1:8
    fscanf(arduin)
end

fwrite(arduin, 'a')

for i=1:13
    fscanf(arduin)
end

figure
hold on
xlabel('Time')
ylabel('Angle (degrees)')
length = 12;
vertex = [0,0];
axis(gca,'equal');
axis([-25 40 -25 40]);

for i = 1:12
    fscanf(arduin);
end
for i = 1:10000 %divide everything by frequency to get real-time plots; graph is based on displaying 100 points
    hold on,

    if length(rawValue) == 28
        rawValue = strcat(rawValue); %concatenate char array into string, and convert string to double
        dataArray = strsplit(rawValue, ', ');
    else
        continue;
    end
    
    theta1 = str2double(dataArray(1));
    theta2 = str2double(dataArray(2));
    theta3 = str2double(dataArray(3));
    
    P2 = length*[cos(theta1), sin(theta1)];
    P3 = length*[cos(theta2), sin(theta2)]+ P2;
    P4 = length*[cos(theta3), sin(theta3)]+ P3;
     
    if mod(i, 2) == 0
        crank1 = line([vertex(1), P2(1)], [vertex(2), P2(2)]);
        crank2 = line([P2(1), P3(1)], [P2(2), P3(2)]);
        crank3 = line([P3(1), P4(1)], [P3(2), P4(2)]);
        pause(0.001);
        delete(crank1);
        delete(crank2);
        delete(crank3);
    end
    
%     if i<100/frequency % for displaying first 100 points
%         axis([0 100/frequency -100 100])
%         htext = text(20/frequency, 100,num2str(angle),'HorizontalAlignment','center');
%     else
%         axis([i-100/frequency i -100 100]) % shift graph
%         htext = text((i-80/frequency), 100, num2str(angle),'HorizontalAlignment','center');
%     end

end
