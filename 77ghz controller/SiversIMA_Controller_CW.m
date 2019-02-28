% This script is to communicate with two SiversIMA Radars and let them work
% in CW mode
% RMIT University 2016

%%
%fclose(SerialPortRadar2);
%fclose(SerialPortRadar1);
clc
close all
clear
%% Paramters

% FREQUENCY:STEP Default: 1e6 (1MHz ) Max: - Min: 8e3 (8kHz)

f1 = 76e9 +  500e6 + 000000 ; % First radar freq
f2 = 76e9 +  500e6 + 008000 ; % Second radar freq
N  = 1000;      % Number of samples
M  = 20;        % Number of samples per period


fb = 1000;       % beat freq
Ts = 1/(fb*M);  % Default: 50e-6 (50 ?s) Min: 50e-6 Max: 1
Time = N*Ts;%N*Ts;     % measurement time in seconds


%% opening Serial port
if (~exist ('SerialPortRadar1'))
    fprintf('Creating serial object for Radar 1\n');
    SerialPortRadar1 = serial('COM4','InputBufferSize',2000,'BaudRate',115200);
    SerialPortRadar1.Timeout = 5;
    if (strcmp(SerialPortRadar1.Status,'closed'))
        fopen(SerialPortRadar1);
    end
    fprintf('Ok\n');
    SerialPortRadar1.Terminator='CR';
    pause(1);
    fprintf('Radar 1 Ready!!\n\n');
elseif (strcmp(SerialPortRadar1.Status,'closed'))
    fprintf('Opening serial ports for Radar 1\n');
    fopen(SerialPortRadar1);
end


if (~exist ('SerialPortRadar2'))
    fprintf('Creating serial object for Radar 2\n');
    SerialPortRadar2 = serial('COM6','InputBufferSize',2000,'BaudRate',115200);
    SerialPortRadar2.Timeout = 5;
    if (strcmp(SerialPortRadar2.Status,'closed'))
        fopen(SerialPortRadar2);
    end
    fprintf('Ok\n');
    SerialPortRadar2.Terminator='CR';
    pause(1);
    fprintf('Radar 2 Ready!!\n\n');
elseif (strcmp(SerialPortRadar2.Status,'closed'))
    fprintf('Opening serial ports for Radar 2\n');
    fopen(SerialPortRadar2);
end

% fclose(SerialPortRadar2);
% fclose(SerialPortRadar1);
%%
flushinput(SerialPortRadar1);

% Intizializing
fprintf(SerialPortRadar1,'INIT');
fprintf(SerialPortRadar2,'INIT');
pause(0.1)
fprintf(['Radar 1 Initializing :', fgetl(SerialPortRadar1)]);
fprintf(['Radar 2 Initializing :', fgetl(SerialPortRadar2)]);

% Setting Frequency parameters
fprintf(SerialPortRadar1,['FREQUENCY:CENTER ',num2str(f1)]);
fprintf(SerialPortRadar2,['FREQUENCY:CENTER ',num2str(f2)]);
pause(0.1)
flushinput(SerialPortRadar1);
flushinput(SerialPortRadar2);
fprintf(SerialPortRadar1,'FREQUENCY:CENTER ?');
fprintf(['Radar 1 Frequency :', fgetl(SerialPortRadar1)]);
fprintf(SerialPortRadar2,'FREQUENCY:CENTER ?');
fprintf(['Radar 2 Frequency :', fgetl(SerialPortRadar2)]);

fprintf(SerialPortRadar1,['FREQUENCY:SPAN 0']);
fprintf(SerialPortRadar2,['FREQUENCY:SPAN 0']);
pause(0.1)
flushinput(SerialPortRadar1);
flushinput(SerialPortRadar2);
fprintf(SerialPortRadar1,'FREQUENCY:SPAN ?');
fprintf(['Radar 1 Span :', fgetl(SerialPortRadar1)]);
fprintf(SerialPortRadar2,'FREQUENCY:SPAN ?');
fprintf(['Radar 2 Span :', fgetl(SerialPortRadar2)]);


% Set the sampling period
fprintf(SerialPortRadar1,['SWEEP:IDLE ',num2str(Ts)]);
fprintf(SerialPortRadar2,['SWEEP:IDLE ',num2str(Ts)]);
pause(0.1)
flushinput(SerialPortRadar1);
flushinput(SerialPortRadar2);
fprintf(SerialPortRadar1,'SWEEP:IDLE  ?');
fprintf(['Radar 1 Sampling Period :', fgetl(SerialPortRadar1)]);
fprintf(SerialPortRadar2,'SWEEP:IDLE  ?');
fprintf(['Radar 2 Sampling Period  :', fgetl(SerialPortRadar2)]);

% Set the number of samples 
fprintf(SerialPortRadar1,['FREQUENCY:POINTS ',num2str(N)]);
fprintf(SerialPortRadar2,['FREQUENCY:POINTS ',num2str(N)]);
pause(0.1)
flushinput(SerialPortRadar1);
flushinput(SerialPortRadar2);
fprintf(SerialPortRadar1,'FREQUENCY:POINTS ?');
fprintf(['Radar 1 Samples :', fgetl(SerialPortRadar1)]);
fprintf(SerialPortRadar2,'FREQUENCY:POINTS ?');
fprintf(['Radar 2 Samples :', fgetl(SerialPortRadar2)]);

% Setting sweep time
fprintf(SerialPortRadar1,['SWEEP:TIME ',num2str(Time)]);
fprintf(SerialPortRadar2,['SWEEP:TIME ',num2str(Time)]);
pause(0.1)
flushinput(SerialPortRadar1);
flushinput(SerialPortRadar2);
fprintf(SerialPortRadar1,'SWEEP:TIME ?');
fprintf(['Radar 1 Time :', fgetl(SerialPortRadar1)]);
fprintf(SerialPortRadar2,'SWEEP:TIME ?');
fprintf(['Radar 2 Time :', fgetl(SerialPortRadar2)]);


%% Triggering the measurments
% Set the number of samples 
N_Spectro = 50; % spectrogram window
Z_Spectro = zeros(N_Spectro,N/2);
while(1)
fprintf(SerialPortRadar1,['TRIGGER:ARM']);
fprintf(SerialPortRadar2,['TRIGGER:ARM']);
pause(1)
flushinput(SerialPortRadar1);
flushinput(SerialPortRadar2);
fprintf(SerialPortRadar2,'TRACE:DATA ?');
for ctr=1:N
B=fscanf(SerialPortRadar2);
A(ctr)=str2num(B);
end

A=A-mean(A);
std(A)
plot (A)
ylim([-1000 1000])
grid on; box on;
drawnow


% FDomaion = fftshift(fft(A));
% Z = FDomaion(1000/2:end-1);
% Z(1:2) = 0 ; % Supress the DC offset
% 
% Z_Spectro = circshift(Z_Spectro,-1,1);
% Z_Spectro(end,:) = Z(1:end)/10;
% image(10*log10(abs(Z_Spectro)));
% drawnow


end