function psdScope(V,I)

ft = 200:200000;

vLoad = V;
iLoad = I;

sampFreq = 625000;
nfft = length(vLoad);

[psdV,f] = pwelch(vLoad,hamming(round(nfft/100)),round(nfft/1000),round(length(vLoad)),sampFreq,'psd');
[psdI,f] = pwelch(iLoad,hamming(round(nfft/100)),round(nfft/1000),round(length(iLoad)),sampFreq,'psd');
psdZ = sqrt(psdV./psdI);

fX = figure;
fY = figure;

figure(fX)
hold on 
grid on

semilogx(f(ft),10*log10(psdV(ft)),'LineWidth',1.5)
semilogx(f(ft),10*log10(psdI(ft)),'--','LineWidth',1.5)
set(gca, 'XScale', 'log')
set(gcf,'color','w');
ax = gca;
ax.FontSize = 28;
ax.FontName = 'Serif';

legend('V_{DUT}(f)','I_{DUT}(f)')
xlabel('Frequency (Hz)')
ylabel('Magnitude (dB)')

hold off

figure(fY)
semilogx(f(ft),20*log10(psdZ(ft)),'LineWidth',1.5)
set(gcf,'color','w');
ax = gca;
ax.FontSize = 25;
ax.FontName = 'Serif';
legend('Z(f)')
xlabel('Frequency (Hz)')
ylabel('Magnitude (dB)')
grid on