<?php

namespace Drupal\redhen_connection\Controller;

use Drupal\Core\Controller\ControllerBase;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Entity\EntityStorageInterface;
use Drupal\Core\Link;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\HttpKernel\Exception\NotFoundHttpException;

/**
 * Class ConnectionAddController.
 *
 * @package Drupal\redhen_connection\Controller
 */
class ConnectionAddController extends ControllerBase {

  /**
   * Initialize entity storage.
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
   *   A render array for a list of the redhen_connection bundles/types that can
   *   be added or if there is only one type/bundle defined for the site,
   *   the function returns the add page for that bundle/type.
   */
  public function add(Request $request) {
    $entity = redhen_connection_get_connection_entity_from_route();

    $types = $this->typeStorage->loadMultiple();

    foreach ($types as $key => $redhen_connection_type) {
      $connection = $redhen_connection_type->getEndpointEntityTypes($entity->getEntityTypeId(), $entity->getType());
      if (empty($connection)) {
        unset($types[$key]);
      }
    }

    if ($types && count($types) == 1) {
      $type = reset($types);
      return $this->addForm($request, $type, $entity);
    }
    if (count($types) === 0) {
      return [
        '#markup' => $this->t('You have not created any %bundle types yet. @link to add a new type.', [
          '%bundle' => 'Connection',
          '@link' => Link::createFromRoute($this->t('Go to the type creation page'), 'entity.redhen_connection_type.add_form'),
        ]),
      ];
    }
    return [
      '#theme' => 'redhen_connection_content_add_list',
      '#content' => $types,
      '#entity' => $entity,
    ];
  }

  /**
   * Provides the page title for add controller.
   *
   * @return string
   *   The page title.
   */
  public function getAddTitle() {
    $entity_type = redhen_connection_get_connection_entity_from_route()->getEntityType();
    return t('Select a @type Connection Type',
      ['@type' => $entity_type->getLabel()]
    );
  }

  /**
   * Creation form for redhen_connection entities of given bundle/type.
   *
   * @param \Symfony\Component\HttpFoundation\Request $request
   *   The current request object.
   * @param \Drupal\Core\Entity\EntityInterface $redhen_connection_type
   *   The custom bundle to add.
   * @param \Drupal\Core\Entity\EntityInterface $entity
   *   The entity for one of the endpoints.
   *
   * @return array
   *   A form array as expected by drupal_render().
   */
  public function addForm(Request $request, EntityInterface $redhen_connection_type) {
    $entity = redhen_connection_get_connection_entity_from_route();
    $endpoint_fields = $redhen_connection_type->getEndpointFields($entity->getEntityTypeId(), $entity->getType());

    if (empty($endpoint_fields)) {
      // No valid endpoint fields found for the provided connection type and
      // entity.
      throw new NotFoundHttpException();
    }

    $connection_entity = $this->storage->create([
      'type' => $redhen_connection_type->id(),
    ]);

    // Grab the first field if we have more than one.
    $field = reset($endpoint_fields);

    // Set the value of the endpoint.
    $connection_entity->set($field, $entity);

    return $this->entityFormBuilder()->getForm($connection_entity, 'default', ['fixed_endpoint' => $field]);
  }

  /**
   * Provides the page title for this controller.
   *
   * @param \Drupal\Core\Entity\EntityInterface $redhen_connection_type
   *   The custom bundle/type being added.
   * @param \Drupal\Core\Entity\EntityInterface $entity
   *   The provided endpoint.
   *
   * @return string
   *   The page title.
   */
  public function getAddFormTitle(EntityInterface $redhen_connection_type) {
    $entity = redhen_connection_get_connection_entity_from_route();
    return t('Create @type Connection for @entity',
      ['@type' => $redhen_connection_type->label(), '@entity' => $entity->label()]
    );
  }

}
