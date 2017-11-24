function [y] = conv_audio(f, g)
    audio_input = f ;
    h = g;
    audio_output = zeros(length(g)+length(f)-1,1);
    for i=1:length(g)+length(f)-1
        for j=1:length(g) 
            if(i-j+1>0 && i-j+1<length(g)) 
                audio_output(i) = audio_output(i) + h(j) * audio_input(i-j+1); 
            else 
            end 
        end 
    end
    y=audio_output;
end
