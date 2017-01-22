length = 12;
vertex = [0 0];

axis(gca,'equal');
axis([-1 25 -1 25]);
for t=1:3081
    hold on,
    theta1 = data(t);
    theta2 = data2(t);
    P2 = length*[cos(theta1), sin(theta1)];
    P3 = length*[cos(theta2), sin(theta2)]+ P2;
    crank1 = line([vertex(1), P2(1)], [vertex(2), P2(2)]);
    crank2 = line([P2(1), P3(1)], [P2(2), P3(2)]);
    pause(0.01);
    delete(crank1);
    delete(crank2);
end
