%upload DataCollect first
%set up serial communication
delete(instrfindall)
bluetoot = bluetooth('HC-06',1);
fclose(bluetoot)
fopen(bluetoot)
frequency = 20;
length = 12;
vertex = [0 0];

for i=1:5
    fscanf(bluetoot)
end

fwrite(bluetoot, 'a')

for i=1:2
    fscanf(bluetoot)
end

figure
hold on
axis(gca,'equal');
axis([-1 25 -1 25]);

for i=1/frequency:1/frequency:1000  %divide everything by frequency to get real-time plots; graph is based on displaying 100 points
    
    data = strcat(fscanf(bluetoot)); %concatenate char array into string, and convert string to double
    dataArray = strsplit(data,',');
    
    theta1 = dataArray(1);
    theta2 = dataArray(2);
    hold on,
    P2 = length*[cos(theta1), sin(theta1)];
    P3 = length*[cos(theta2), sin(theta2)]+ P2;
    crank1 = line([vertex(1), P2(1)], [vertex(2), P2(2)]);
    crank2 = line([P2(1), P3(1)], [P2(2), P3(2)]);
    pause(0.01);
    delete(crank1);
    delete(crank2);

end