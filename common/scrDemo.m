% scrDemo
close all;
clearvars;
clc;

global FIGTITLE demo BPFrm datBits fs ctScrDirDir;
global SYNC; 
global FIGWID FIGHGT TXSCR RXSCR SCRSZ;
global FIGXOFF FIGYOFF FIGXDLT FIGYDLT;
global TXRRSCLF;

% set paths
ctFileCodeSrc = [mfilename('fullpath') '.m'];                           % get fullpath of current file
[ctScrDir,~,~] = fileparts(ctFileCodeSrc);                              % get scripts dir
[ctScrDirDir,~,~] = fileparts(ctScrDir);                              % get scripts dir
cd(ctScrDir);
fs = filesep;
fs = ['\' fs];                                                           % set scripts dir as pwd (reference)
rng('Default');

FIGTITLE = 'Off';
SYNC = 1;       % 1: transmit 0: receive
FIGWID = 560;
FIGHGT = 420;
TXSCR = 0;
RXSCR = 1;
SCRSZ = get(0,'ScreenSize');
FIGXOFF = 96+FIGWID; FIGXDLT = 24;
FIGYOFF = 0; FIGYDLT = 96;

% Other signal Config
TXRRSCLF = 1;

% % ----OOK----
% fprintf('--OOK--\n');
% fSig = 1e6;       % NOTE: For fSig~=1MHz, upsampling signal to DAC sampling rates increases required buffer size on FPGA beyond allocation - thus causing transmit signal chaain to fail.
% fPlt = 1e6;       % NOTE: For fPlt~=1MHz, upsampling signal to DAC sampling rates increases required buffer size on FPGA beyond allocation - thus causing transmit signal chaain to fail.
% spFrm = 8;
% demo = cDemoOOK(fSig, fPlt, spFrm);
% BPFrm = spFrm;
% %------------

% ----OFDM----
fprintf('--OFDM--\n');
fSig = 5e6;       % NOTE: For fSig~=1MHz, upsampling signal to DAC sampling rates increases required buffer size on FPGA beyond allocation - thus causing transmit signal chaain to fail.
fPlt = 5e6;       % NOTE: For fPlt~=1MHz, upsampling signal to DAC sampling rates increases required buffer size on FPGA beyond allocation - thus causing transmit signal chaain to fail.
spFrm = 1;
demo = cDemoOFDM(fSig, fPlt, spFrm);
BPFrm = spFrm*demo.mod.BPSYM;
%------------

% initialize buffer to hold data bits for BER calculation
datBits = cFIFO(BPFrm);
datBits(2) = cFIFO(BPFrm);
datBits(3) = cFIFO(BPFrm);
datBits(4) = cFIFO(BPFrm);
% for i=2:4
%     datBits(i) = cFIFO(BPFrm);
% end
%--------------------------------

% Start Transmit Routine
txTmr = timer('Name','StartTxClt',...
              'StartDelay', 2,...
              'Period', 0.25,...
              'TasksToExecute', 4, ...
              'ExecutionMode', 'fixedRate',...
              'StartFcn', @demoTxTimer,...
              'StopFcn', @demoTxTimer,...
              'TimerFcn', @demoTxTimer);
start(txTmr);
%--------------------------------

% Start Receive Routine
rxTmr = timer('Name','StartRxClt',...
              'StartDelay', 2,...
              'Period', 0.5,...
              'TasksToExecute', 7,...
              'ExecutionMode', 'fixedRate',...
              'StartFcn', @demoRxTimer,...
              'StopFcn', @demoRxTimer,...
              'TimerFcn', @demoRxTimer);
start(rxTmr);
% %--------------------------------

%--END--