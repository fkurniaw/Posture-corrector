length = 6;
vertex = [0 0];

axis(gca,'equal');
axis([-1 25 -1 25]);
for t=1:3081
    hold on,
    theta1 = angles_4(t, 1);
    theta2 = angles_4(t, 2);
    theta3 = angles_4(t, 3);
    theta4 = angles_4(t, 4);
    P2 = length*[cos(theta1), sin(theta1)];
    P3 = length*[cos(theta2), sin(theta2)]+ P2;
    P4 = length*[cos(theta3), sin(theta3)]+ P3;
    P5 = length*[cos(theta4), sin(theta4)]+ P4;
    crank1 = line([vertex(1), P2(1)], [vertex(2), P2(2)]);
    crank2 = line([P2(1), P3(1)], [P2(2), P3(2)]);
    crank3 = line([P3(1), P4(1)], [P3(2), P4(2)]);
    crank4 = line([P4(1), P5(1)], [P4(2), P5(2)]);
    pause(0.005);
    delete(crank1);
    delete(crank2);
    delete(crank3);
    delete(crank4);
end
