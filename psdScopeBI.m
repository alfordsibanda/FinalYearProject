function psdScopeBI(PRBSin,PRISin)

ft = 200:200000;

PRBs = PRBSin;
PRIs = PRISin;

sampFreq = 625000;
nfft = length(PRBs);

[psdB,f] = pwelch(PRBs,hamming(round(nfft/100)),round(nfft/1000),round(length(PRBs)),sampFreq,'psd');
[psdI,f] = pwelch(PRIs,hamming(round(nfft/100)),round(nfft/1000),round(length(PRIs)),sampFreq,'psd');

fX = figure;

figure(fX)
hold on 
grid on

semilogx(f(ft),10*log10(psdB(ft)),'LineWidth',1.5)
semilogx(f(ft),10*log10(psdI(ft)),'--','LineWidth',1.5)
set(gca, 'XScale', 'log')
set(gcf,'color','w');
ax = gca;
ax.FontSize = 28;
ax.FontName = 'Serif';

legend('PRBS(f)','PRIS(f)')
xlabel('Frequency (Hz)')
ylabel('Magnitude (dB)')

hold off