
clear all;
%clc;
tic

% MODIFY HERE
numOfStages=64; %how many stages does one Arbiter PUF have
numOfXors=1; %How many XORs are used (i.e. how many Arbiter PUFs are xored)
numOfInputs=1000; %How many different challenge and response paris are generated


challengeBitsMaster=randi(2,numOfInputs,numOfStages,'int8')-1; %Generate random challenges with one bit per entry
%load('challengeBitsMaster');
for(currInput=1:numOfInputs)
    if (numOfStages == 64) 
       [c seq]=LFSR(challengeBitsMaster(currInput,:),[64 63 61 60], numOfStages);
    else 
      [c seq]=LFSR(challengeBitsMaster(currInput,:),[128 126 101 99], numOfStages);  
    end
    
    challengeBitsAll((currInput -1)*numOfStages + 1:(currInput - 1)*numOfStages+numOfStages,:)=c;
    %challengeBitsAll(1:64,:)=c;
end

numOfInputs = numOfInputs * numOfStages;
challengeMatrixUint=zeros(numOfStages/8+1,numOfInputs,'uint8'); %Allocate memory for a more space optimized way to store the challenges with 8 bit per byte, and store it in a way to model the puf using n+1 delay values


% Transform the challenges so that you can model a PUF using n+1 parameters
for(j=1:numOfStages-1)
  challengeMatrix(j,:)=1-(mod(sum(challengeBitsAll(:,j:numOfStages)'),2)*2);
end
challengeMatrix(numOfStages,:)=1-(mod(challengeBitsAll(:,numOfStages),2)*2);
challengeMatrix(numOfStages+1,:)=1;


% Do the same again. But this time we also store the results more efficiently 
% by storing 8 challenge bits into one byte. My optimized mex function
% needs this as an input. So this is only needed for the optimized mex
% function
currIndex=1;
for(j=1:numOfStages)
    challengeMatrixUint(currIndex,:)=challengeMatrixUint(currIndex,:)*2;
    challengeMatrixUint(currIndex,:)=challengeMatrixUint(currIndex,:)+uint8(1-(mod(sum(challengeBitsAll(:,j:numOfStages)'),2)));
  if(mod(j,8)==0)
    currIndex=currIndex+1;
  end
end

challengeMatrixUint(numOfStages/8+1,:)=1;


delayVector=zeros(numOfXors,numOfStages+1,'double'); %the delay vectyors of the PUFs with n+1 parameters

%MODIFY HERE FOR DELAY
%Generate PUF instances and compute the ChallengeArray for the n+1
%simulation option
for(currXor=1:numOfXors)
        % Generate a PUF using random parameters following a normal
        % distribution
       % delayArray=double(normrnd(0,1,numOfStages,2)); %The delay difference for challenge 0 and challenge 1
     %   delayArray= intDelayArray;
      % delayArray= int16(1000*delayArray);
      load('delayArray')
        delayArrayAllXors(currXor,:,:)=delayArray;
        
        %transform the 2*n parameters to n+1 parameter according to the
        %formula from the paper and store it in delayVector
        delayVector(currXor,1)=delayArray(1,1)-delayArray(1,2); 
      for(j=2:numOfStages)
       delayVector(currXor,j)=delayArray(j-1,1)+delayArray(j-1,2)+delayArray(j,1)-delayArray(j,2);
      end
    delayVector(currXor,numOfStages+1)=delayArray(numOfStages,1)+delayArray(numOfStages,2);
    
    load('delayVector')
end



%Option 1: Compute the responses using the original 2*n delay option
%This implementation a very slow and unoptimized! 
%You should try to avoid If clases ,Do it with multiplications and additions. 
%The matrix multiplication is magnitudes faster!
outputArray=logical(zeros(numOfXors,numOfInputs)); %the final output
outputXorArray=logical(zeros(1,numOfInputs)); %the output of the individual arbiter PUFs

for(currXor=1:numOfXors) %For every Arbiter PUF in your XOR PUF constuct
   
        for(currInput=1:numOfInputs)
        currDelay=0;
        for(currStage=1:numOfStages) 
            if(challengeBitsAll(currInput,currStage)==0)
               currDelay=currDelay+delayArrayAllXors(currXor,currStage,1); 
            else
                   %the wires switched, hence the dealy is now inverted:
                   currDelay=currDelay*(-1);
                   currDelay=currDelay+delayArrayAllXors(currXor,currStage,2);
            end
            currDelayPerStage(currStage)=currDelay;

        end
        if(currDelay>0)
            outputArray(currXor,currInput)=1;
        else
            outputArray(currXor,currInput)=0;
        end
        
        %If we use an XOR arbiter PUF we need to XOR the current PUF
        %response with the PUF response from the other PUFs
        %Tipp: We do not have an efficient btiwise XOR operation in matlab
        % but A~=B is the same as A xor B
        outputXorArray(currInput)=outputXorArray(currInput)~=outputArray(currXor,currInput);
    end
end


outputArray2=logical(zeros(numOfXors,numOfInputs)); %the final output
outputXorArray2=logical(zeros(1,numOfInputs)); %the output of the individual arbiter PUFs

% An optimized implementation using the 
%Generate PUF instances and compute the response
for(currXor=1:numOfXors)

     %Compute the delay differences
   currDelay2=delayVector(currXor,:)*challengeMatrix(:,:);
    outputArray2(currXor,:)=currDelay2>0;
    % You can compute the response efficiently with the mex function I
    % wrote in c. The mex file needs to be in the same folder for it to
    % work 
	% outputArray2(currXor,:)=comPUFwLUT(delayVector(currXor,:)',challengeMatrixUint(:,:));

     
    %Xor the response bit of the current PUF with the responses of the previous PUF for an XOR PUF    
    outputXorArray2=outputArray2(currXor,:)~=outputXorArray2;
end

%outputXorArray should be identical with outputXorArray2 and contain the
%final responses
sum(abs(outputXorArray2-outputXorArray))

%You can store the values by simply typing:
% save('FileName')

%and then load using
% load('FileName')

csvwrite('challenges.csv',challengeBitsMaster);
csvwrite('reference.csv',outputArray2);
disp('all done')