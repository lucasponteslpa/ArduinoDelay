[y,Fs] = audioread('C:\Users\lpa\Downloads\Napalm Death - You Suffer (1) (online-audio-converter.com).wav');
h = zeros(43392,1);
h(1) = 1;
h(21696) = 1/2;
w = conv(h,y(:,1));