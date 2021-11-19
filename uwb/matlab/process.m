
data1 = load_meas('/home/hollosi//projects/tmit/vitmma07/vitmma07-gyak/uwb/matlab/example.meas','0001');
data2 = load_meas('/home/hollosi//projects/tmit/vitmma07/vitmma07-gyak/uwb/matlab/example.meas','0002');

data1 = data1(:,2:end)
data2 = data2(:,2:end);

figure;
subplot(2,1,1);
plot_tx(data1);
title('TX of BABA');

subplot(2,1,2);
plot_tx(data2);
title('TX of C0DE');

figure;
subplot(2,1,1);
plot_freq_error(data1);
title('Frequecy error of BABA');

subplot(2,1,2);
plot_freq_error(data2);
title('Frequecy error of C0DE');

figure;
subplot(2,1,1);
plot_ss_twr_distance(data1);
title('SS TWR distance of BABA');

subplot(2,1,2);
plot_ss_twr_distance(data2);
title('SS TWR distance of C0DE');

figure;
subplot(2,1,1);
plot_ads_twr_distance(data1);
title('ADS TWR distance of BABA');

subplot(2,1,2);
plot_ads_twr_distance(data2);
title('ADS TWR distance of C0DE');











