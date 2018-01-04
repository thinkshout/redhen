<?php

namespace Drupal\redhen_connection;

use Drupal\Core\Entity\EntityTypeInterface;
use Drupal\Core\Entity\EntityTypeManagerInterface;
use Drupal\Core\StringTranslation\StringTranslationTrait;

/**
 * Provides the redhen_connection views integration.
 */
class ViewsData {

  use StringTranslationTrait;

  /**
   * The entity type manager.
   *
   * @var \Drupal\Core\Entity\EntityTypeManagerInterface
   */
  protected $entityTypeManager;

  /**
   * The connection service.
   *
   * @var \Drupal\redhen_connection\ConnectionServiceInterface
   */
  protected $connections;

  /**
   * Creates a new ViewsData instance.
   *
   * @param \Drupal\Core\Entity\EntityTypeManagerInterface $entity_type_manager
   *   The entity type manager.
   * @param \Drupal\redhen_connection\ConnectionServiceInterface $connections
   *   The connection service.
   */
  public function __construct(EntityTypeManagerInterface $entity_type_manager, ConnectionServiceInterface $connections) {
    $this->entityTypeManager = $entity_type_manager;
    $this->connections = $connections;
  }

  /**
   * Returns the views data.
   *
   * @return array
   *   The views data.
   */
  public function getViewsData() {
    $data = [];

    $data['redhen_connection'] = [];

    $data['redhen_connection']['table'] = [];

    $data['redhen_connection']['table']['group'] = $this->t('Connection');

    $data['redhen_connection']['table']['provider'] = 'redhen_connection';

    // Add a join for each entity type to the redhen_connection table.
    foreach ($this->connections->getConnectionEntityTypes($this->entityTypeManager->getDefinitions()) as $connection_type_id => $endpoints) {
      foreach ($endpoints as $endpoint_id => $entity_types) {
        foreach ($entity_types as $entity_type_id => $entity_type) {
          /** @var \Drupal\views\EntityViewsDataInterface $views_data */
          // We need the views_data handler in order to get the table name later.
          if ($this->entityTypeManager->hasHandler($entity_type_id, 'views_data') && $views_data = $this->entityTypeManager->getHandler($entity_type_id, 'views_data')) {
            // Add a join from the entity base table to the redhen connection table.
            $base_table = $views_data->getViewsTableForEntityType($entity_type);
            $data['redhen_connection']['table']['join'][$base_table] = [
              'left_field' => $entity_type->getKey('id'),
              'field' => $endpoint_id,
            ];

            $data['redhen_connection']["{$entity_type_id}__{$endpoint_id}_{$connection_type_id}"] = [
              'relationship' => [
                'id' => 'standard',
                'label' => $this->t('@label connection', ['@label' => $entity_type->getLabel()]),
                'title' => $entity_type->getLabel(),
                'help' => t('The related @entity_type for @endpoint_id from @connection_type.', ['@entity_type' => $entity_type->getLowercaseLabel(), '@endpoint_id' => str_replace('_', ' ', $endpoint_id), '@connection_type' => $connection_type_id]),
                'base' => $this->getEndpointViewsTableForEntityType($entity_type),
                'base field' => $entity_type->getKey('id'),
                'argument table' => 'redhen_connection',
                'argument field' => 'status',
                'relationship field' => $endpoint_id,
                'extra' => [
                  [
                    'left_field' => 'type',
                    'value' => $connection_type_id,
                  ],
                ],
                'filter' => [
                  'handler' => '\Drupal\redhen_connection\Plugin\views\HandlerFilterStatus'
                ],
              ],
            ];
          }
        }
      }
    }

    return $data;
  }

  /**
   * Gets the table of an entity type to be used as endpoint table in views.
   *
   * @param \Drupal\Core\Entity\EntityTypeInterface $entity_type
   *   The entity type.
   *
   * @return string
   *   The base table.
   */
  protected function getEndpointViewsTableForEntityType(EntityTypeInterface $entity_type) {
    return $entity_type->getDataTable() ?: $entity_type->getBaseTable();
  }

}
