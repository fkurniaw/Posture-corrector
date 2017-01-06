time = [0 .001 .002 .003];
time2 = 0:.001:.003;
A = [.001, .002, .003; 1 2 3];      % Can use spaces or commas to separate columns

time(1);    % first index is 1, not 0
time(end);  % last element

1+2;
time(2)^2;
time = 0:0.001:1;

y = time.^2;
big = y > 0.5;


plot(time, y);
plot(time, big);    % true/false 

class(time)         % which type is time?
class(big)

big(1) = 2.3;       
big(1);             % prints 1, can only be 0 or 1

ind = find(y > 0.5);    % returns all indices that are true
firstind = find(y > 0.5, 1, 'first');    % returns first index that is true (the 1 says print 1)
lastind = find(y > 0.5, 2, 'last');

length(ind);
time(1);
time(1:3);
time(ind);

77
figure, hold on, plot(time, y), plot(time(ind), y(ind), 'r');   % hold on keeps previous plot on the figure, multiple plots on 1 figure
figure, subplot(3, 1, 1), plot(time, y);    % 3x1, index 1

figure, subplot(3, 1, 1), plot(time, y), hold on, plot(time(ind), y(ind), 'r');
subplot(3, 1, 2), plot(time, y > 0.5);
subplot(3, 1, 3), plot(time(firstind), 0, 'rx');

gca;    % get current axes
help gca;
set(gca, 'XLim', [0 1]);
set(gca, 'XLim', [min(time) max(time)]);
xlabel('Time (s)');
title('This');


% save('time-squared.mat', 'time', 'y');
% clear all;
% load('time-squared.mat');

