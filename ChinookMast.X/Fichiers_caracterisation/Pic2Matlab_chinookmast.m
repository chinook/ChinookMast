%Credits:    Fred Chassé
%Adaptation: Jerome Pare-Lepine
%Date:       10 mars 2017

close all

% Wait for the user to start the program
reply = 'N';
while reply == 'N'
  reply = upper(input('Start? Y/N [N]:','s'));
  if isempty(reply)
    reply = 'Y'
  end
end

% Create a serial port object.
port = instrfind('Type', 'serial', 'Port', 'COM3', 'Tag', '');

if isempty(port)
    port = serial('COM3');
else
    fclose(port);
    port = port(1)
end

n = 1;

% port.BytesAvailableFcnCount = 32*4; % 30 floats to receive = 120 bytes
port.BytesAvailableFcnCount = 2*4; % size of the whole string (see StateSendData)
port.BytesAvailableFcnMode = 'byte';
port.BytesAvailableFcn = {@myCallback};

port.BaudRate = 57600; %Down from 115200, to match skadi specs
port.DataBits = 8;
port.Parity = 'none';
port.StopBits = 1;
port.Terminator = '';

% Connect to instrument object, port.
fopen(port);

fwrite(port, 'g')

% Create figure for data
fig1 = figure(1);
hold on

% Wait for the user to stop the program
reply = 'N';
while reply == 'N'
  reply = input('Stop? Y/N [Y]:','s');
  if isempty(reply)
    reply = 'Y'
  end
end

fwrite(port, 's')
% Disconnect from instrument object, obj1.
fclose(port);

% Clean up all objects.
delete(port);

% Extract data from figure
axesObjs = get(fig1, 'Children');
dataObjs = get(axesObjs, 'Children');
xdata = get(dataObjs, 'XData');
ydata = get(dataObjs, 'YData');
ndata = length(xdata);

x1 = [xdata{ndata}];
y1 = [ydata{ndata}];
for i = ndata - 1 : -1 : 1
  x1 = [x1 xdata{i}];
  y1 = [y1 ydata{i}];
end

figure(2)
plot(x1, y1)
legend(['Data'])


