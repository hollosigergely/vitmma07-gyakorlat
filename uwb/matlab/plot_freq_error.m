function plot_freq_error(data)
    CNT_TO_SEC=1/(499.2e6*128);
    
    ddata = cdiff(data(2:end,:),data(1:end-1,:));
    ddata = ddata * CNT_TO_SEC;

    plot((ddata(:,1)-ddata(:,2))./ddata(:,1)*10^6);
    ylabel('ppm');
    grid;
end