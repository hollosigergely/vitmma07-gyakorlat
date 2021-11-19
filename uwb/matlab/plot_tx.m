function plot_tx(data)
    CNT_TO_SEC=1/(499.2e6*128);
    
    ddata = cdiff(data(2:end,:),data(1:end-1,:));
    ddata = ddata * CNT_TO_SEC;

    plot(ddata(:,1));
    grid;
end