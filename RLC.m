frequency = 15000;      % PRBS frequency
Tclock = 1/frequency;   % Clock frequency
impulseLen = 1000e-6;   % Longest PRBS run (Rounded up)
PRBSrun = 14;           % PRBS length design

syms R L C tau1 tau2 s1 s2; % Declaring symbolic variables
X = 5; % Set the number of time constants for tau2 in eqn3

tau1 = 0.01*Tclock; % Time constant is equal to 1% of period
% Increasing tau1 increases the inductor size
% tau2 is set by eqn3 due to constraints
% tau2 is left as an unknown to not overconstrain the equations

C = 2e-6; % Setting a capacitor
% L or C  can be fixed in order to calculate other parameters

s1 = 1/tau1;
s2 = 1/tau2;

% The roots of a series RLC circuit. (CURRENT)
eqn1 = s1 == (-R/(2 * L)) + sqrt(((C * R.^2) -(4 * L) )/(4 * C * L.^2)) ;
eqn2 = s2 == (-R/(2 * L)) - sqrt(((C * R.^2) -(4 * L) )/(4 * C * L.^2)) ;

% Longest run of a PRBS signal. It has to rise and fall in that duration
% eqn3 = (5*tau1)+ (X*tau2) == impulseLen;
eqn3 = (5*tau1)+ (X*tau2) == (PRBSrun*Tclock);

% Solving system of equations
sol = solve([eqn1, eqn2, eqn3],[R L tau2]);

Resistor = sol.R;
Inductor = sol.L;
t2 = double(sol.tau2);

% The solutions 
resistorR = double(abs(sol.R)); % Ohm - RLC Resitor 
inductorL = double(abs(sol.L)); % Henry - RLC Inductor
%capacitorC = abs(sol.C); % Farad - RLC Capacitor 
capacitorC = C; % Farad - RLC Capacitor


