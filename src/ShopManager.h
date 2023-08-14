#pragma once

#include "ItemManager.h"

class CShopManager
{
public:
	CShopManager();
	~CShopManager();

	void Init();
	void Shutdown();
	void LoadProducts();
	bool KVToJson();
	void OnShopPacket(CReceivePacket* msg, CExtendedSocket* socket);
	void GetProductBySubId(int productId, Product& product, SubProduct& subProduct);
	bool BuyProduct(CUser* user, int productTypeId, int productId);
	void InsertProduct(Product product);

	std::vector<Product> m_Products;
	std::vector<std::vector<int>> m_RecommendedProducts;
	std::vector<int> m_PopularProducts;
};
