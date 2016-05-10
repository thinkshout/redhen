<?php

/**
 * @file
 * Contains \Drupal\redhen_relation\Entity\Relation.
 */

namespace Drupal\redhen_relation\Entity;

use Drupal\views\EntityViewsData;
use Drupal\views\EntityViewsDataInterface;

/**
 * Provides Views data for Relation entities.
 */
class RelationViewsData extends EntityViewsData implements EntityViewsDataInterface {
  /**
   * {@inheritdoc}
   */
  public function getViewsData() {
    $data = parent::getViewsData();

    // Unset the default entity relationships.
    // It does not work properly, the target type it is not defined.
    unset($data['redhen_relation']['entity1']['relationship']);
    unset($data['redhen_relation']['entity2']['relationship']);

    // Collect all relationable entity types.
    $relation_types = RelationType::loadMultiple();
    $entity_type_ids = [];
    /** @var \Drupal\redhen_relation\RelationTypeInterface $relation_type */
    foreach ($relation_types as $relation_type) {
      if ($entity_type_id = $relation_type->getEndpointEntityTypeId('1')) {
        $entity_type_ids[] = $entity_type_id;
      }
      if ($entity_type_id = $relation_type->getEndpointEntityTypeId('2')) {
        $entity_type_ids[] = $entity_type_id;
      }
    }
    $entity_type_ids = array_unique($entity_type_ids);

    // Provide a relationship for each entity type found.
    foreach ($entity_type_ids as $entity_type_id) {
      /** @var \Drupal\Core\Entity\EntityTypeInterface $entity_type */
      $entity_type = $this->entityManager->getDefinition($entity_type_id);
      $data['redhen_relation'][$entity_type_id] = [
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
