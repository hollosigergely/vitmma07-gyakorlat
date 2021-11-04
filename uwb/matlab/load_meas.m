function ret = load_meas(filename, anchor)
  fprintf(1,'Load data (filename: %s)\n', filename);  

  fileID = fopen(filename);
  Cmeas = textscan(fileID,'%s %u %u %u64','delimiter',' ');
  fclose(fileID);

  ret=[];
  for k=1:length(Cmeas{1})
    if ~strcmp(Cmeas{1}(k),anchor)
      continue;
    end
    
    trid=double(Cmeas{2}(k));
    msgid=Cmeas{3}(k);
    ts=Cmeas{4}(k);
    
    if length(ret) == 0
      rowidx = [];
    else
      rowidx = find(ret(:,1) == trid);      
    end
    
    if length(rowidx) > 1
      fprintf(1,'WARNING: Multiple trid!'); 
    end
       
    if length(rowidx) == 0
      row = [ trid, 0, 0, 0, 0, 0, 0 ];
      rowidx = size(ret,1)+1;
    else
      row = ret(rowidx,:);
    end
            
    ANTDELAY = 16414;
    if mod(msgid,2) == 0
        ts = ts + ANTDELAY;
    else
        ts = ts - ANTDELAY;
    end
    row(msgid+2) = ts;
    ret(rowidx,:) = row;
  end

  ret = ret(all(ret(:,2:end),2),:);
  
  fprintf(1,'Read %d transactions\n', size(ret,1));  
end