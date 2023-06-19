-------------------------------------------------------------------------------------------------
local imageutils = {}
local ffi = package.preload.ffi()  
-------------------------------------------------------------------------------------------------

function loadimage(goname, imagefilepath, tid )

	--print(gltfobj.basepath..v.uri)
	local res, err = image.load(sys.load_resource( imagefilepath ))
	if(err) then print("[Image Load Error]: "..v.uri.." #:"..err) end 

	-- TODO: This goes into image loader
	if(res.buffer ~= "") then
		rgbcount = 3
		if(res.type == "rgba") then res.format = resource.TEXTURE_FORMAT_RGBA; rgbcount = 4 end
		if(res.type == "rgb") then res.format = resource.TEXTURE_FORMAT_RGB; rgbcount = 3 end

		local buff = buffer.create(res.width * res.height, { 
			{	name=hash(res.type), type=buffer.VALUE_TYPE_UINT8, count=rgbcount } 
		})
		local stm = buffer.get_stream(buff, hash(res.type))
		-- for idx = 1, v.res.width * v.res.height * rgbcount do 
		-- 	stm[idx] = string.byte(v.res.buffer, idx )
		-- end
		geomextension.setbufferbytes( buff, res.type, res.buffer )

		res.type=resource.TEXTURE_TYPE_2D	
		res.num_mip_maps=1

		local resource_path = go.get(goname, "texture"..tid)

		-- Store the resource path so it can be used later 
		res.resource_path = resource_path
		res.image_buffer = buff 

		resource.set_texture( resource_path, res, buff )
		go.set(goname, "texture"..tid, resource_path)

		msg.post( goname, hash("mesh_texture") )
	end

	return res
end 

-------------------------------------------------------------------------------------------------

function bufferimage(goname, stringbuffer, size, tid, restype )

	-- Default to rgb and albedo
	tid = tid or 0
	restype = restype or "rgb"
	assert(stringbuffer)
	
	local res = { buffer = stringbuffer, type = restype, width = size, height = size }

	-- TODO: This goes into image loader
	if(res.buffer ~= "") then
		rgbcount = 3
		if(res.type == "rgba") then res.format = resource.TEXTURE_FORMAT_RGBA; rgbcount = 4 end
		if(res.type == "rgb") then res.format = resource.TEXTURE_FORMAT_RGB; rgbcount = 3 end

		local buff = buffer.create(res.width * res.height, { 
			{	name=hash(res.type), type=buffer.VALUE_TYPE_UINT8, count=rgbcount } 
		})
		local stm = buffer.get_stream(buff, hash(res.type))
		-- for idx = 1, v.res.width * v.res.height * rgbcount do 
		-- 	stm[idx] = string.byte(v.res.buffer, idx )
		-- end
		geomextension.setbufferbytes( buff, res.type, res.buffer )

		res.type=resource.TEXTURE_TYPE_2D	
		res.num_mip_maps=1

		local resource_path = go.get(goname, "texture"..tid)

		-- Store the resource path so it can be used later 
		if(res.resourcepath) then res.resource_path = resource_path end
		res.image_buffer = buff 

		resource.set_texture( resource_path, res, buff )
		go.set(goname, "texture"..tid, resource_path)

		msg.post( goname, hash("mesh_texture") )
	end

	return res
end 

-------------------------------------------------------------------------------------------------

function bufferresource( goname, size, tid, restype, init )

	-- Default to rgb and albedo
	init = init or 0
	tid = tid or 0
	restype = restype or "rgb"
	
	local res = { buffer = nil, type = restype, width = size, height = size }
	
	rgbcount = 3
	if(restype == "rgba") then res.format = resource.TEXTURE_FORMAT_RGBA; rgbcount = 4 end
	if(restype == "rgb") then res.format = resource.TEXTURE_FORMAT_RGB; rgbcount = 3 end

	local buffsize = res.width * res.height * rgbcount
	-- res.buffer = string.rep(string.char(init), buffsize)
	local strmem = ffi.new("unsigned char[?]", buffsize+1, 0xff)
	res.buffer = ffi.string(strmem, buffsize+1)
	
	local buff = buffer.create(res.width * res.height, { 
		{	name=hash(res.type), type=buffer.VALUE_TYPE_UINT8, count=rgbcount } 
	})
	geomextension.setbufferbytes( buff, res.type, res.buffer )

	res.type=resource.TEXTURE_TYPE_2D	
	res.num_mip_maps=1

	local resource_path = go.get(goname, tid)

	-- Store the resource path so it can be used later 
	res.resource_path = resource_path 
	res.image_buffer = buff 

	resource.set_texture( resource_path, res, buff )
	go.set(goname, tid, resource_path)

	msg.post( goname, hash("mesh_texture") )

	return res
end 

-------------------------------------------------------------------------------------------------

imageutils.loadimage 		= loadimage
imageutils.bufferimage 		= bufferimage
imageutils.bufferresource 	= bufferresource

-------------------------------------------------------------------------------------------------

return imageutils

-------------------------------------------------------------------------------------------------