% This script is to communicate with SiversIMA Radar
% RMIT University 2016

%%
clc
close all

%% opening Serial port
if (~exist ('FMCWRadar'))
    fprintf('Creating serial object\n');
    SerialPortRadar = serial('COM4','InputBufferSize',2000,'BaudRate',115200);
    SerialPortRadar.Timeout = 5;
    if (strcmp(SerialPortRadar.Status,'closed'))
        fopen(SerialPortRadar);
    end
    fprintf('Ok\n');
    SerialPortRadar.Terminator='CR';
    pause(1);
    fprintf('Ready!!\n\n');
elseif (strcmp(SerialPortRadar.Status,'closed'))
    fprintf('Opening serial ports\n');
    fopen(SerialPortRadar);
end
% fclose(SerialPortRadar);
%%
    flushinput(SerialPortRadar);
fprintf(SerialPortRadar,'INIT');
pause(0.1)
fgetl(SerialPortRadar)
fprintf(SerialPortRadar,'SWEEP:MEASURE ON')
pause(0.1)
fgetl(SerialPortRadar)
fprintf(SerialPortRadar,'SWEEP:NUMBERS 5')
pause(0.1)
fgetl(SerialPortRadar)
%%
%fgetl(SerialPortRadar)
N=1000;
N_Spectro = 50; % spectrogram window
Z_Spectro = zeros(N_Spectro,N/2);
while(1)
    flushinput(SerialPortRadar);
fprintf(SerialPortRadar,'TRIG:ARM')
pause(0.2)
fgetl(SerialPortRadar)
fprintf(SerialPortRadar,'TRACE:DATA ?');
%pause(0.5)
for ctr=1:N
B=fscanf(SerialPortRadar);
A(ctr)=str2num(B);
end

%

%plot(abs(FDomaion(2:end)))

FDomaion = fftshift(fft(A));
Z = FDomaion(1000/2:end-1);
Z(1:2) = 0 ; % Supress the DC offset

Z_Spectro = circshift(Z_Spectro,-1,1);
Z_Spectro(end,:) = Z(1:end)/10;
image(10*log10(abs(Z_Spectro)));
drawnow
end