#pragma once

class IShopManager : public IBaseManager
{
public:
	virtual bool LoadProducts() = 0;

	virtual void OnShopPacket(CReceivePacket* msg, IExtendedSocket* socket) = 0;
	virtual void GetProductBySubId(int productId, Product& product, SubProduct& subProduct) = 0;
	virtual bool BuyProduct(IUser* user, int productTypeId, int productId) = 0;
	
	virtual const std::vector<Product>& GetProducts() = 0;
	virtual const std::vector<std::vector<int>>& GetRecommendedProducts() = 0;
	virtual const std::vector<int>& GetPopularProducts() = 0;
};
