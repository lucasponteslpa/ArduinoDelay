[y,Fs] = audioread('C:\Users\lpa\Downloads\Napalm Death - You Suffer.wav');
h = zeros(Fs/2,1);
h(1) = 1;
h(Fs/2) = 1/2;
%w = conv(h,y(:,1));
audio_out = zeros(size(h) + size(y(1))-1, 1);

for tal =1 :(size(h) + size(y(1))-1)
    for t = 1:size(h)
        if(t-tal+1>0)
            audio_out(t) = audio_out(t) + h(t)*y(t-tal+1,1);
        else
        end
    end
end

