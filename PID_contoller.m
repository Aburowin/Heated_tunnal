global tmpInstant Euerr e_1
% input DT, E_P, E_I, E_D
inputdlg({'DT (C)','K_P','K_I','K_D'});
DT_set = str2num(ans{1}); E_P = str2num(ans{2});
E_I = str2num(ans{3}); E_D = str2num(ans{4});
DT_daq = daq('dt'); % Initiate DAQ
% Initiate arduino board
arduinoA1 = serialport('com10',115200);
% Add channel 0 for freestream thermocouple
addinput(DT_daq,"DT9828(00)",0,'Voltage');
% Add channel 1 for thermocouple 1 on plate
addinput(DT_daq,"DT9828(00)",1,'Voltage');
% Add channel 2 for thermocouple 2 on plate
addinput(DT_daq,"DT9828(00)",2,'Voltage');
% Set the sampling rate and filter window size
samplingRate = 8; filterWin = 3;
DT_daq.Rate = samplingRate;
flush(DT_daq); % Refresh the DAQ
DT_daq.ScansAvailableFcnCount = samplingRate * filterWin;
% Start simultaneously acquiring the data
[tmpInstant, ~, ~] = read(src, ...
    src.ScansAvailableFcnCount, "OutputFormat","Matrix");
e_1 = 0; Euerr = 0; % Initiate parameters for the PID
start(DT_daq,"Continuous");pause(0.05); % Start the DAQ
load('temp_coeff.mat'); % Load calibration
while DT_daq.Running
    % Freestream temperature
    T_free = polyval(coeff1,tmpInstant(:,1));
    % Thermocouple 1 on plate
    Temp1 = polyval(coeff2,tmpInstant(:,2));
    % Thermocouple 2 on plate
    Temp2 = polyval(coeff3,tmpInstant(:,3));
    T_wall_set = T_free + DT_set; % Wall set temp
    % The wall avg temperature
    TmpCon = (mean(Temp1) + mean(Temp2))/2;
    % Difference between set and actual temperature
    diff_act_set = T_wall_set - TmpCon;
    % Duty cycle from the PID
    O = PID_heater(K_P, K_I, K_D, DT_set, diff_act_set);
    inputPWM = round(O*255); % The PWM value
    % Signal to arduino to set the PWM
    writeline(arduinoA1,['5,' num2str(inputPWM)]);
end
% PID sub-function
function O = PID_heater(K_P, K_I, K_D,...
    DT_set, diff_act_set)
global Euerr e_1
% Error between set and actual temp
e = diff_act_set/DT_set;
% Duty cycle calculation
O = K_P*e + K_I*Euerr + K_D*(e - e_1);
if O >= 1
    O = 1;
elseif O <= 0
    O = 0;
end
if (e*DT_set >= 0.4)
    c_inv = 1;
elseif (e*DT_set >= 0)
    c_inv = 0.5;
else
    c_inv = 0;
end
Euerr = Euerr + c_inv*e;
e_1   = e;
end