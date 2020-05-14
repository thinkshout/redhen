<?php

namespace Drupal\redhen_connection\Controller;

use Drupal\Component\Render\FormattableMarkup;
use Drupal\Core\Controller\ControllerBase;
use Drupal\Core\Entity\EntityStorageInterface;
use Drupal\Core\Link;
use Drupal\Core\Url;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Symfony\Component\HttpFoundation\Request;

/**
 * Class RedhenConnections.
 *
 * @package Drupal\redhen_connection\Controller
 */
class RedhenConnections extends ControllerBase {

  /**
   * {@inheritdoc}
   */
  public function __construct(EntityStorageInterface $storage, EntityStorageInterface $type_storage) {
    $this->storage = $storage;
    $this->typeStorage = $type_storage;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    /** @var \Drupal\Core\Entity\EntityTypeManagerInterface $entity_type_manager */
    $entity_type_manager = $container->get('entity_type.manager');
    return new static(
      $entity_type_manager->getStorage('redhen_connection'),
      $entity_type_manager->getStorage('redhen_connection_type')
    );
  }

  /**
   * Displays add links for available bundles/types for redhen_connection.
   *
   * @param \Symfony\Component\HttpFoundation\Request $request
   *   The current request object.
   *
   * @return array
   *   A table of connections that are made to this content.
   */
  public function list(Request $request) {
    /** @var ConnectionServiceInterface $connection_service */
    $connection_service = \Drupal::service('redhen_connection.connections');
    $entity = redhen_connection_get_connection_entity_from_route();
    $entity_type_key = $entity->getEntityTypeId();
    $connections = $connection_service->getConnections($entity);

    // Creates the table header.
    $header = [
      'Type',
      'Name',
      'Operations',
    ];
    $add_url = Url::fromRoute("$entity_type_key.connection.add_page", [$entity_type_key => $entity->id()], ['absolute' => TRUE])->toString();

    $rows = [];
    foreach ($connections as $connection) {
      $view = Link::createFromRoute($connection->label()->render(), 'entity.redhen_connection.canonical', ['redhen_connection' => $connection->id()])->toString();
      $edit = Link::createFromRoute('Edit', 'entity.redhen_connection.edit_form', ['redhen_connection' => $connection->id()])->toString();
      $delete = Link::createFromRoute('Delete', 'entity.redhen_connection.delete_form', ['redhen_connection' => $connection->id()])->toString();
      $row = [
        'data' => [
          $connection->getType(),
          new FormattableMarkup("@view", ['@view' => $view]),
          new FormattableMarkup(
            "@edit @delete", ['@edit' => $edit, '@delete' => $delete]),
        ],
      ];
      $rows[] = $row;
    }
    // Build the table.
    // @todo add actual link for adding a connection to this the given entity.
    $build = [
      'table'           => [
        '#theme'         => 'table',
        '#prefix' => "<ul class='action-links'><li><a class='button button-action button--primary button--small' href=" . $add_url . ">Add Connection</a></li></ul>",
        '#attributes'    => [
          'data-striping' => 0,
        ],
        '#header' => $header,
        '#rows'   => $rows,
      ],
    ];

    return $build;
  }

}
