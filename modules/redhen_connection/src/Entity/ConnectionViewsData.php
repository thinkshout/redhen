<?php

namespace Drupal\redhen_connection\Entity;

use Drupal\views\EntityViewsData;
use Drupal\views\EntityViewsDataInterface;

/**
 * Provides Views data for Connection entities.
 */
class ConnectionViewsData extends EntityViewsData implements EntityViewsDataInterface {
  /**
   * {@inheritdoc}
   */
  public function getViewsData() {
    $data = parent::getViewsData();

    // Unset the default entity relationships.
    // It does not work properly, the target type it is not defined.
    unset($data['redhen_connection']['entity1']['relationship']);
    unset($data['redhen_connection']['entity2']['relationship']);

    // Collect all connectionable entity types.
    $connection_types = ConnectionType::loadMultiple();
    $entity_type_ids = [];
    /** @var \Drupal\redhen_connection\ConnectionTypeInterface $connection_type */
    foreach ($connection_types as $connection_type) {
      if ($entity_type_id = $connection_type->getEndpointEntityTypeId('1')) {
        $entity_type_ids[] = $entity_type_id;
      }
      if ($entity_type_id = $connection_type->getEndpointEntityTypeId('2')) {
        $entity_type_ids[] = $entity_type_id;
      }
    }
    $entity_type_ids = array_unique($entity_type_ids);

    // Provide a relationship for each entity type found.
    foreach ($entity_type_ids as $entity_type_id) {
      /** @var \Drupal\Core\Entity\EntityTypeInterface $entity_type */
      $entity_type = $this->entityManager->getDefinition($entity_type_id);
      $data['redhen_connection'][$entity_type_id] = [
        'relationship' => [
          'title' => $entity_type->getLabel(),
          'help' => t('The related @entity_type.', ['@entity_type' => $entity_type->getLowercaseLabel()]),
          'base' => $entity_type->getDataTable() ?: $entity_type->getBaseTable(),
          'base field' => $entity_type->getKey('id'),
          'relationship field' => 'related_entity',
          'id' => 'standard',
          'label' => $entity_type->getLabel(),
        ],
      ];
    }

    return $data;
  }

}
