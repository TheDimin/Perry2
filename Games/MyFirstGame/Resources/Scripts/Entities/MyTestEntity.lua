local MyEntity = {};


function MyEntity:CalculateVelocity(oldVelocity)
  local inputSource = Vec3(Input:GetAxis("MoveHorizontal"),0,-Input:GetAxis("MoveVertical"))

 
  inputSource = inputSource:Normalize()
  MyEntity.LastDir = inputSource;

  if(Input:GetAction("Dash")) then
    inputSource = inputSource * Vec3(50) * Vec3(Engine:DeltaTime())
  end
  print(inputSource.x .. " :: "..inputSource.z)
  return inputSource + inputSource
end

return MyEntity