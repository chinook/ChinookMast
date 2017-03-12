function myCallback(obj,event)

bytesAvailable = obj.BytesAvailable;
rawData = fread(obj, 2*4);
% rawData = fread(obj, bytesAvailable);
valuesReceived = obj.ValuesReceived;

% time = event.Data.AbsTime;
% tMax = time(5) + time(6)/60;
% tMin = tMax - 30*.05/60;
% x = tMin + .05/60 : .05/60 : tMax;
% x = 0 : 2*pi/32 : 2*pi - 2*pi/32;

% Convert UINT8 to float
data = typecast(typecast(uint8(rawData), 'uint32'), 'single');
nData = length(data);
xdata = data(1:2:end);
ydata = data(2:2:end);

% Add data to figure
fig1 = figure(1);
plot(xdata, ydata, 'b')
plot(data(1:nData/2), data(nData/2 + 1:nData), 'b')
% plot(x, data, 'b')

end % myCallback