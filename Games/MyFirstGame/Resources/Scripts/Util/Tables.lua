function rPrint(s, l, i) -- recursive Print (structure, limit, indent)
	l = (l) or 100; i = i or "";	-- default item limit, indent string
	if (l<1) then print "ERROR: Item limit reached."; return l-1 end;
	local ts = type(s);
	if (ts ~= "table") then print (i,ts,s); return l-1 end
	print (i,ts);           -- print "table"
	for k,v in pairs(s) do  -- print "[KEY] VALUE"
		l = rPrint(v, l, i.."\t["..tostring(k).."]");
		if (l < 0) then break end
	end
	return l
end	

--https://stackoverflow.com/questions/31452871/table-getn-is-deprecated-how-can-i-get-the-length-of-an-array
function getTableSize(t)
    local count = 0
    for _, __ in pairs(t) do
        count = count + 1
    end
    return count
end


--https://stackoverflow.com/questions/640642/how-do-you-copy-a-lua-table-by-value
function shallowCopy(t)
	local t2 = {}
	for k,v in pairs(t) do
	  t2[k] = v
	end
	return t2
  end